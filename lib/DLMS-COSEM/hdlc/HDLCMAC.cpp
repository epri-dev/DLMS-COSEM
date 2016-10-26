#include <algorithm>
#include <iostream>
#include <memory>

#include "ISerial.h"
#include "hdlc/HDLCMAC.h"

namespace EPRI
{
    HDLCMAC::HDLCMAC(const HDLCAddress& MyAddress, 
        ISerial * pSerial, 
        const HDLCOptions& Options,
        uint8_t MaxPreallocatedPacketBuffers) :
        m_MyAddress(MyAddress),
        m_pSerial(pSerial),
        m_CurrentOptions(Options)
    {
        while (MaxPreallocatedPacketBuffers--)
        {
            m_Packets.push(std::unique_ptr<Packet>(new Packet));
        }
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

    HDLCErrorCode HDLCMAC::ProcessSerialReception()
    {
        uint8_t		   Byte;
        HDLCErrorCode  ReturnValue = NEED_MORE;
        Packet *       pPacket = nullptr;
        ERROR_TYPE     ReadRet;
        uint32_t       CharacterTimeout = 0;

        while (m_pSerial->Read(&Byte, sizeof(Byte), CharacterTimeout) == SUCCESSFUL)
        {
            if (!pPacket)
            {
                pPacket = GetWorkingRXPacket();
                if (!pPacket)
                {
                    return NO_PACKETS;
                }
                //
                // We've locked on, now we need to wait for the ICT.
                //
                CharacterTimeout = m_CurrentOptions.InterOctetTimeoutInMs;
            }
            ReturnValue = pPacket->MakeByByte(Byte);
            if (ReturnValue != NEED_MORE)
            {
                break;
            }
        }
    	
        if (SUCCESS == ReturnValue)
        {
            EnqueueWorkingRXPacket();            
        }
        else
        {
            ReleaseWorkingRXPacket();
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

    HDLCErrorCode HDLCMAC::ProcessSerialTransmission()
    {
        HDLCErrorCode  ReturnValue = NEED_MORE;
        Packet *       pTXPacket = GetOutgoingPacket();
        if (pTXPacket)
        {
            if (m_pSerial->Write(*pTXPacket, pTXPacket->GetPacketLength()) != SUCCESSFUL) 
            {
                ReturnValue = TX_FAILURE;
                //
                // RETRY - STILL IN TX QUEUE
                //
            }
            ReleaseOutgoingPacket();
        }
        return ReturnValue;
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

} /* namespace EPRI */