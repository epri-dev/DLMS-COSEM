#include <unistd.h>

#include "IBaseLibrary.h"
#include "IScheduler.h"
#include "hdlc/HDLCMAC.h"
#include "HDLCLLC.h"

namespace EPRI
{
    ///////////////////////////////////////////////////////////////////////////
    // 
    // Client LLC
    //
    ///////////////////////////////////////////////////////////////////////////
    //
    HDLCClientLLC::HDLCClientLLC(const HDLCAddress& MyAddress, 
        ISerialSocket * pSerial, 
        const HDLCOptions& Options,
        uint8_t MaxPreallocatedPacketBuffers /* = 10 */) :
        HDLCLLC(&m_MAC),
        m_MAC(MyAddress, pSerial, Options, MaxPreallocatedPacketBuffers)
    {
        m_MAC.RegisterCallback(DLConnectConfirmOrResponse::ID,
            std::bind(&HDLCClientLLC::MACConnectConfirm, this, std::placeholders::_1));
        m_MAC.RegisterCallback(DLDataRequestParameter::ID,
            std::bind(&HDLCClientLLC::MACDataIndication, this, std::placeholders::_1));
        m_MAC.RegisterCallback(DLIdentifyResponseParameter::ID,
            std::bind(&HDLCClientLLC::MACIdentifyConfirm, this, std::placeholders::_1));
    }
    
    HDLCClientLLC::~HDLCClientLLC()
    {
    }
    //
    // IDENTIFY Service Implementation
    //
    bool HDLCClientLLC::IdentifyRequest(const DLIdentifyRequestParameter& Parameters)
    {
        return m_MAC.IdentifyRequest(Parameters);
    }
    
    void HDLCClientLLC::RegisterIdentifyConfirm(CallbackFunction Callback)
    {
        RegisterCallback(DLIdentifyResponseParameter::ID, Callback);
    }    

    bool HDLCClientLLC::MACIdentifyConfirm(const BaseCallbackParameter& Parameters)
    {
        bool    RetVal = false;
        FireCallback(DLIdentifyResponseParameter::ID, Parameters, &RetVal);
        return true;
    }
    //
    // DA-CONNECT Service Implementation
    //
    bool HDLCClientLLC::ConnectRequest(const DLConnectRequestOrIndication& Parameters)
    {
        return m_MAC.ConnectRequest(Parameters);
    }
    
    void HDLCClientLLC::RegisterConnectConfirm(CallbackFunction Callback)
    {
        RegisterCallback(DLConnectConfirmOrResponse::ID, Callback);
    }

    bool HDLCClientLLC::MACConnectConfirm(const BaseCallbackParameter& Parameters)
    {
        bool    RetVal = false;
        FireCallback(DLConnectConfirmOrResponse::ID, Parameters, &RetVal);
        
        return MACConnectConfirmOrIndication(Parameters);
    }
    //
    // DA-DATA Service Implementation
    //
    void HDLCClientLLC::RegisterDataIndication(CallbackFunction Callback)
    {
        RegisterCallback(DLDataRequestParameter::ID, Callback);
    }
    //
    // Transport
    //
    bool HDLCClientLLC::MACDataIndication(const BaseCallbackParameter& Parameters)
    {
        bool                          RetVal = false;
        FireCallback(DLDataRequestParameter::ID, Parameters, &RetVal);
        return HDLCLLC::MACDataIndication(Parameters);
    }
    //
    ///////////////////////////////////////////////////////////////////////////
    // 
    // Client MAC
    //
    ///////////////////////////////////////////////////////////////////////////
    //
    HDLCClient::HDLCClient(const HDLCAddress& MyAddress, 
        ISerialSocket * pSerial, 
        const HDLCOptions& Opt,
        uint8_t MaxPreallocatedPacketBuffers) :
        HDLCMAC(MyAddress, pSerial, Opt, MaxPreallocatedPacketBuffers)
    {
        m_PacketCallback.RegisterCallback(HDLCControl::UI, 
            std::bind(&HDLCClient::UI_Handler, this, std::placeholders::_1));
        m_PacketCallback.RegisterCallback(HDLCControl::UA, 
            std::bind(&HDLCClient::UA_Handler, this, std::placeholders::_1));
        m_PacketCallback.RegisterCallback(HDLCControl::INFO, 
            std::bind(&HDLCClient::I_Handler, this, std::placeholders::_1));
        m_PacketCallback.RegisterCallback(HDLCControl::IDENTR, 
            std::bind(&HDLCClient::IDENTR_Handler, this, std::placeholders::_1));
    }
    
    HDLCClient::~HDLCClient()
    {
    }
    //
    // IDENTIFY Service
    //
    bool HDLCClient::IdentifyRequest(const DLIdentifyRequestParameter& Parameters)
    {
        bool ReturnValue = false;
        BEGIN_TRANSITION_MAP
            TRANSITION_MAP_ENTRY(ST_DISCONNECTED, ST_DISCONNECTED)
            TRANSITION_MAP_ENTRY(ST_IEC_CONNECT, EVENT_IGNORED)
            TRANSITION_MAP_ENTRY(ST_CONNECTING_WAIT, EVENT_IGNORED)
            TRANSITION_MAP_ENTRY(ST_CONNECTED, EVENT_IGNORED)
        END_TRANSITION_MAP(ReturnValue, new IdentifyEventData(Parameters));
        return ReturnValue;
    }
    //
    // DA-CONNECT Service Implementation
    //
    bool HDLCClient::ConnectRequest(const DLConnectRequestOrIndication& Parameters)
    {
        bool RetVal = false;
        if (ST_DISCONNECTED == m_CurrentState)
        {
            EventData * pData = new ConnectEventData(Parameters);
            if (m_CurrentOptions.StartWithIEC)
            {
                RetVal = ExternalEvent(ST_IEC_CONNECT, pData);
            }
            else
            {
                RetVal = ExternalEvent(ST_CONNECTING_WAIT, pData);
            }
        }
        return RetVal;
    }
        
    void HDLCClient::Process()
    {
        Base()->GetScheduler()->Post(std::bind(&HDLCClient::ProcessSerialTransmission, this));
    }

    void HDLCClient::ST_Disconnected_Handler(EventData * pData)
    {
        //
        // IDENTIFY Request
        //
        IdentifyEventData * pIdentifyEvent = dynamic_cast<IdentifyEventData *>(pData);
        if (pIdentifyEvent)
        {
            Packet *      pIDENT = GetWorkingTXPacket();
            if (pIDENT)
            {
                HDLCErrorCode ReturnCode = 
                    pIDENT->MakeIdentifyPacket(HDLCControl(HDLCControl::IDENT));
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
        // IDENTR 
        //
        PacketEventData * pPacketData = dynamic_cast<PacketEventData *>(pData);
        if (pPacketData && pPacketData->Data.GetControl().PacketType() == HDLCControl::IDENTR)
        {
            bool    RetVal = false;
            size_t  InformationLength = 0;
            //
            // Packet Parsing Garauntees Correct Information Sizing
            //
            FireCallback(DLIdentifyResponseParameter::ID, 
                DLIdentifyResponseParameter(m_MyAddress, pPacketData->Data.GetInformation(InformationLength)),
                &RetVal);
            
            Process();
            return;
        }        
    }

    void HDLCClient::ST_IEC_Connect_Handler(EventData * pData)
    {
        //
        // IEC Request
        //
        ISerial::Options    IECOptions(ISerial::Options::BAUD_300, 7, ISerial::Options::PARITY_EVEN);
        if (m_pSerial->SetOptions(IECOptions) == SUCCESS &&
            m_pSerial->Write(DLMSVector((const uint8_t *)("/?!\r\n"), 5)) == SUCCESS)
        {
            // TODO - BETTER!
            const uint8_t MODE_E_9600[] = { 0x06, 0x32, 0x35, 0x32, 0x0D, 0x0A };
            DLMSVector    Data;

            enum IECState
            {
                STATE_ID,
                STATE_BAUD,
                STATE_HDLC
            }          CurrentState = STATE_ID;
            while (STATE_HDLC != CurrentState)
            {
                switch (CurrentState)
                {
                case STATE_ID:
                    {
                        ERROR_TYPE Result = m_pSerial->Read(&Data, 1, 2000);
                        if (SUCCESSFUL != Result)
                        {
                            CurrentState = STATE_BAUD;
                        }
                        else
                        {
                            if (Data.PeekByteAtEnd() == '\n' && Data.PeekByteAtEnd(sizeof(uint8_t) == '\r'))
                            {
                                CurrentState = STATE_BAUD;
                                break;
                            }
                        }
                    }
                    break;
                case STATE_BAUD:
                    m_pSerial->Write(DLMSVector(MODE_E_9600, sizeof(MODE_E_9600)));
                    Base()->GetScheduler()->Sleep(1000);
                    m_pSerial->SetOptions(
                        ISerial::Options(ISerial::Options::BAUD_9600,
                        8, 
                        ISerial::Options::PARITY_NONE,
                        ISerial::Options::STOPBITS_ONE));
                    m_pSerial->Flush(ISerialSocket::FlushDirection::BOTH);
                    CurrentState = STATE_HDLC;
                    InternalEvent(ST_CONNECTING_WAIT, pData);
                    break;
                        
                default:
                    break;
                }
            }
        }
        
        Process();
    }

    void HDLCClient::ST_Connecting_Wait_Handler(EventData * pData)
    {
        //
        // Connect Request
        //
        ConnectEventData * pConnectData = dynamic_cast <ConnectEventData *>(pData);
        if (pConnectData)
        {
            Packet *      pSNRM = GetWorkingTXPacket();
            if (pSNRM)
            {
                HDLCErrorCode ReturnCode = pSNRM->MakePacket(Packet::NO_SEGMENT,
                    pConnectData->Data.DestinationAddress,
                    m_MyAddress,
                    HDLCControl(HDLCControl::SNRM));
                if (SUCCESS == ReturnCode)
                {
                    EnqueueWorkingTXPacket();
                }
                else
                {
                    ReleaseWorkingTXPacket();
                    InternalEvent(ST_DISCONNECTED);
                }
            }
            Process();
            return;
        }

    }
    
    void HDLCClient::ST_Connected_Handler(EventData * pData)
    {
        //
        // UA 
        //
        PacketEventData * pPacketData = dynamic_cast<PacketEventData *>(pData);
        if (pPacketData && pPacketData->Data.GetControl().PacketType() == HDLCControl::UA)
        {
            bool RetVal = false;
            FireCallback(DLConnectConfirmOrResponse::ID, 
                DLConnectConfirmOrResponse(pPacketData->Data.GetSourceAddress()),
                &RetVal);
            
            Process();
            return;
        }
        //
        // Default Handler - Handles Data
        //
        HDLCMAC::ST_Connected_Handler(pData);
        
        Process();
    }
    //
    // Packet Reception Handlers
    //
    bool HDLCClient::UI_Handler(const Packet& RXPacket)
    {
        return true;
    }
    
    bool HDLCClient::UA_Handler(const Packet& RXPacket)
    {
        bool bAllowed = false;
        BEGIN_TRANSITION_MAP
            TRANSITION_MAP_ENTRY(ST_DISCONNECTED, EVENT_IGNORED)
            TRANSITION_MAP_ENTRY(ST_IEC_CONNECT, EVENT_IGNORED)
            TRANSITION_MAP_ENTRY(ST_CONNECTING_WAIT, ST_CONNECTED)
            TRANSITION_MAP_ENTRY(ST_CONNECTED, EVENT_IGNORED)
        END_TRANSITION_MAP(bAllowed, new PacketEventData(RXPacket));
        return bAllowed;
    }

    bool HDLCClient::IDENTR_Handler(const Packet& RXPacket)
    {
        bool bAllowed = false;
        BEGIN_TRANSITION_MAP
            TRANSITION_MAP_ENTRY(ST_DISCONNECTED, ST_DISCONNECTED)
            TRANSITION_MAP_ENTRY(ST_IEC_CONNECT, EVENT_IGNORED)
            TRANSITION_MAP_ENTRY(ST_CONNECTING_WAIT, EVENT_IGNORED)
            TRANSITION_MAP_ENTRY(ST_CONNECTED, EVENT_IGNORED)
        END_TRANSITION_MAP(bAllowed, new PacketEventData(RXPacket));
        return bAllowed;
    }

}