#include <algorithm>
#include <iostream>
#include <memory>

#include "ISerial.h"
#include "hdlc/HDLCMAC.h"

namespace EPRI
{
    HDLCMAC::HDLCMAC(const HDLCAddress& MyAddress, 
        ISerialSocket * pSerial, 
        const HDLCOptions& Options,
        uint8_t MaxPreallocatedPacketBuffers) :
        m_MyAddress(MyAddress),
        m_pSerial(pSerial),
        m_CurrentOptions(Options),
        StateMachine(ST_MAX_STATES)
    {
        //
        // State Machine
        //
        BEGIN_STATE_MAP
            STATE_MAP_ENTRY(ST_DISCONNECTED, HDLCMAC::ST_Disconnected_Handler)
            STATE_MAP_ENTRY(ST_IEC_CONNECT, HDLCMAC::ST_IEC_Connect_Handler)
            STATE_MAP_ENTRY(ST_CONNECTING_WAIT, HDLCMAC::ST_Connecting_Wait_Handler)
            STATE_MAP_ENTRY(ST_CONNECTED, HDLCMAC::ST_Connected_Handler)
        END_STATE_MAP
        //
        while (MaxPreallocatedPacketBuffers--)
        {
            m_Packets.push(std::unique_ptr<Packet>(new Packet));
        }
        //
        // Physical Layer Handlers
        //
        m_pSerial->RegisterConnectHandler(
            std::bind(&HDLCMAC::Serial_Connect, this, std::placeholders::_1));
        m_pSerial->RegisterReadHandler(
            std::bind(&HDLCMAC::Serial_Receive, this, std::placeholders::_1, std::placeholders::_2));
        pSerial->RegisterCloseHandler(
            std::bind(&HDLCMAC::Serial_Close, this, std::placeholders::_1));
        //
        // Packet Handlers
        //
        m_PacketCallback.RegisterCallback(HDLCControl::INFO, 
            std::bind(&HDLCMAC::I_Handler, this, std::placeholders::_1));
        
    }
    
    HDLCMAC::~HDLCMAC()
    {
    }
    
    HDLCAddress HDLCMAC::MyAddress() const
    {
        return m_MyAddress;
    }

    const HDLCStatistics& HDLCMAC::Statistics() const
    {
        return m_Statistics;
    }
    void HDLCMAC::ClearStatistics()
    {
        m_Statistics.Clear();
    }
    
    bool HDLCMAC::IsConnected() const
    {
        return m_CurrentState == ST_CONNECTED;
    }   
    //
    // DA-DATA Service Implementation
    //
    bool HDLCMAC::DataRequest(const DLDataRequestParameter& Parameters)
    {
        bool bAllowed = false;
        BEGIN_TRANSITION_MAP
            TRANSITION_MAP_ENTRY(ST_DISCONNECTED, EVENT_IGNORED)
            TRANSITION_MAP_ENTRY(ST_IEC_CONNECT, EVENT_IGNORED)
            TRANSITION_MAP_ENTRY(ST_CONNECTING_WAIT, EVENT_IGNORED)
            TRANSITION_MAP_ENTRY(ST_CONNECTED, ST_CONNECTED)
        END_TRANSITION_MAP(bAllowed, new DataEventData(Parameters));
        return bAllowed;
    }
    
    void HDLCMAC::ST_Connected_Handler(EventData * pData)
    {
        //
        // Data Request
        //
        DataEventData * pDataEvent = dynamic_cast<DataEventData *>(pData);
        if (pDataEvent)
        {
            Packet *           pInfo = GetWorkingTXPacket();
            if (pInfo)
            {
                HDLCErrorCode ReturnCode = pInfo->MakePacket(Packet::NO_SEGMENT,
                    pDataEvent->Data.DestinationAddress,
                    m_MyAddress,
                    HDLCControl(HDLCControl::INFO),
                    pDataEvent->Data.Data.GetData(),
                    pDataEvent->Data.Data.Size());
                if (SUCCESS == ReturnCode)
                {
                    EnqueueWorkingTXPacket();
                }
                else
                {
                    ReleaseWorkingTXPacket();
                }
            }
            Process();
            return;
        }
        //
        // I Packet
        //
        PacketEventData * pPacketData = dynamic_cast<PacketEventData *>(pData);
        if (pPacketData && pPacketData->Data.GetControl().PacketType() == HDLCControl::INFO)
        {
            bool            RetVal = false;
            size_t          InfoLength = 0;
            const uint8_t * pInformation = pPacketData->Data.GetInformation(InfoLength);
            
            FireCallback(DLDataRequestParameter::ID, 
                DLDataRequestParameter(pPacketData->Data.GetSourceAddress(),
                    HDLCControl::INFO,
                    DLMSVector(pInformation, InfoLength)),
                &RetVal);
            
            Process();
            return;
        }        
    }    
    //
    // Packet Handlers
    //
    bool HDLCMAC::I_Handler(const Packet& RXPacket)
    {
        bool bAllowed = false;
        BEGIN_TRANSITION_MAP
            TRANSITION_MAP_ENTRY(ST_DISCONNECTED, EVENT_IGNORED)
            TRANSITION_MAP_ENTRY(ST_IEC_CONNECT, EVENT_IGNORED)
            TRANSITION_MAP_ENTRY(ST_CONNECTING_WAIT, EVENT_IGNORED)
            TRANSITION_MAP_ENTRY(ST_CONNECTED, ST_CONNECTED)
        END_TRANSITION_MAP(bAllowed, new PacketEventData(RXPacket));
        return bAllowed;
    }

    void HDLCMAC::LockPackets()
    {
        while (m_PacketLock.test_and_set())
            ;
    }
    
    void HDLCMAC::UnlockPackets()
    {
        m_PacketLock.clear();
    }
    
    Packet * HDLCMAC::GetWorkingRXPacket()
    {
        LockPackets();
        if (!m_pRXPacket)
        {
            if (m_Packets.size())
            {
                m_pRXPacket = std::move(m_Packets.front());
                m_Packets.pop();
            }
        }
        UnlockPackets();
        return m_pRXPacket.get();
    }
    
    void HDLCMAC::ReleaseWorkingRXPacket()
    {
        LockPackets();
        if (m_pRXPacket)
        {
            m_pRXPacket->Clear();
            m_Packets.push(std::move(m_pRXPacket));
        }
        UnlockPackets();
    }
    
    void HDLCMAC::EnqueueWorkingRXPacket()        
    {
        LockPackets();
        if (m_pRXPacket)
        {
            m_RXPackets.push(std::move(m_pRXPacket));
        }
        UnlockPackets();
    }

    HDLCErrorCode HDLCMAC::ProcessSerialReception(ERROR_TYPE Error, size_t BytesReceived)
    {
        HDLCErrorCode  ReturnValue = NEED_MORE;
        
        if (ERR_TIMEOUT == Error && !BytesReceived)
        {
            ReleaseWorkingRXPacket();
            m_RXVector.Clear();
            ArmAsyncRead();
        }
        else if (m_pSerial->AppendAsyncReadResult(&m_RXVector, BytesReceived))
        {
            Packet * pPacket = GetWorkingRXPacket();
            if (pPacket)
            {
                ReturnValue = pPacket->MakeByVector(&m_RXVector);
                if (SUCCESS == ReturnValue)
                {
                    EnqueueWorkingRXPacket(); 
                    ProcessPacketReception();
                    m_RXVector.Clear();
                    ArmAsyncRead();
                }
                else if (NEED_MORE == ReturnValue)
                {
                    ArmAsyncRead(m_CurrentOptions.InterOctetTimeoutInMs);
                }
                else
                {
                    //
                    // Parsing Error.  Try to flush and resync.
                    //
                    ReleaseWorkingRXPacket();
                    m_RXVector.Clear();
                    m_pSerial->Flush(EPRI::ISerialSocket::RECEIVE);
                    ArmAsyncRead();
                }          
            }
        }
        return ReturnValue;
    }

    Packet * HDLCMAC::GetWorkingTXPacket()
    {
        LockPackets();
        if (!m_pTXPacket)
        {
            if (m_Packets.size())
            {
                m_pTXPacket = std::move(m_Packets.front());
                m_Packets.pop();
            }
        }
        UnlockPackets();
        return m_pTXPacket.get();
    }
    
    void HDLCMAC::ReleaseWorkingTXPacket()
    {
        LockPackets();
        if (m_pTXPacket)
        {
            m_pTXPacket->Clear();
            m_Packets.push(std::move(m_pTXPacket));
        }
        UnlockPackets();
    }
    
    void HDLCMAC::EnqueueWorkingTXPacket()        
    {
        LockPackets();
        if (m_pTXPacket)
        {
            m_TXPackets.push(std::move(m_pTXPacket));
        }
        UnlockPackets();
    }

    Packet * HDLCMAC::GetOutgoingPacket()        
    {
        Packet * pRetVal = nullptr;
        LockPackets();
        if (m_TXPackets.size())
        {
            pRetVal = m_TXPackets.front().get();
        }
        UnlockPackets();
        return pRetVal;
    }

    void HDLCMAC::ReleaseOutgoingPacket()
    {
        LockPackets();
        if (m_TXPackets.size())
        {
            m_Packets.push(std::move(m_TXPackets.front()));
            m_TXPackets.pop();
        }
        UnlockPackets();
    }

    void HDLCMAC::ProcessSerialTransmission()
    {
        Packet *       pTXPacket = GetOutgoingPacket();
        if (pTXPacket)
        {
            if (m_pSerial->Write(*pTXPacket) != SUCCESSFUL) 
            {
                //
                // RETRY - STILL IN TX QUEUE
                //
            }
            ReleaseOutgoingPacket();
        }
    }

    Packet * HDLCMAC::GetIncomingPacket()
    {
        Packet * pRetVal = nullptr;
        LockPackets();
        if (m_RXPackets.size())
        {
            pRetVal = m_RXPackets.front().get();
        }
        UnlockPackets();
        return pRetVal;
    }
    
    void HDLCMAC::ReleaseIncomingPacket()
    {
        LockPackets();
        if (m_RXPackets.size())
        {
            m_Packets.push(std::move(m_RXPackets.front()));
            m_RXPackets.pop();
        }
        UnlockPackets();
    }

    HDLCErrorCode HDLCMAC::ProcessPacketReception()
    {
        HDLCErrorCode  ReturnValue = NEED_MORE;
        Packet *       pRXPacket = GetIncomingPacket();
        if (pRXPacket)
        {
            HDLCControl::Control PacketType = pRXPacket->GetControl().PacketType();
            if (pRXPacket->GetDestinationAddress() == m_MyAddress)
            {
                bool CallbackRetVal = false;
                if (m_PacketCallback.FireCallback(PacketType, *pRXPacket, &CallbackRetVal) && !CallbackRetVal)
                {
                    ReturnValue = FAIL;
                }
                else
                {
                    ReturnValue = SUCCESS;
                }
            }
            ReleaseIncomingPacket();
        }
        return ReturnValue;
    }
    
    bool HDLCMAC::ArmAsyncRead(uint32_t TimeOutInMs /*= 0*/, size_t MinimumSize /* = sizeof(uint8_t) */)
    {
        return SUCCESSFUL == m_pSerial->Read(nullptr, MinimumSize, TimeOutInMs);
    }
    
    void HDLCMAC::Serial_Receive(ERROR_TYPE Error, size_t BytesReceived)
    {
        ProcessSerialReception(Error, BytesReceived);
    }
    
    void HDLCMAC::Serial_Connect(ERROR_TYPE Error)
    {
        if (!Error)
        {
            //
            // Arm read to catch a header at least
            //
            ArmAsyncRead();
        }
    }
    
    void HDLCMAC::Serial_Close(ERROR_TYPE Error)
    {
    }
    
} /* namespace EPRI */