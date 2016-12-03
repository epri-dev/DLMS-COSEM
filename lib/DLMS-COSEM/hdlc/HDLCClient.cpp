#include <unistd.h>

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
        ISerial * pSerial, 
        const HDLCOptions& Options,
        uint8_t MaxPreallocatedPacketBuffers /* = 10 */) :
        HDLCLLC(&m_MAC),
        m_MAC(MyAddress, pSerial, Options, MaxPreallocatedPacketBuffers)
    {
    }
    
    HDLCClientLLC::~HDLCClientLLC()
    {
    }

    bool HDLCClientLLC::IsConnected() const
    {
        return !m_ConnectedAddress.IsEmpty();
    }
    
    HDLCAddress HDLCClientLLC::ConnectedAddress() const
    {
        return m_ConnectedAddress;
    }

    //
    // DA-CONNECT Service Implementation
    //
    HDLCRunResult HDLCClientLLC::ConnectRequest(const DLConnectRequestOrIndication& Parameters)
    {
        if (m_MAC.ConnectRequest(Parameters))
        {
            return m_MAC.Process();
        }
        return NOTHING_TO_DO;
    }
    
    void HDLCClientLLC::RegisterConnectConfirm(CallbackFunction Callback)
    {
        m_MAC.RegisterCallback(DLConnectConfirmOrResponse::ID,
            std::bind(&HDLCClientLLC::MACConnectConfirm, this, std::placeholders::_1));
        RegisterCallback(DLConnectConfirmOrResponse::ID, Callback);
    }

    bool HDLCClientLLC::MACConnectConfirm(const BaseCallbackParameter& Parameters)
    {
        bool RetVal = false;
        if (FireCallback(DLConnectConfirmOrResponse::ID, Parameters, &RetVal) && RetVal)
        {
            FireTransportEvent(Transport::TRANSPORT_CONNECTED);
            m_ConnectedAddress = dynamic_cast<const DLConnectConfirmOrResponse&>(Parameters).DestinationAddress;
        }
        return RetVal;
    }
    //
    // DA-DATA Service Implementation
    //
    HDLCRunResult HDLCClientLLC::DataRequest(const DLDataRequestParameter& Parameters)
    {
        if (IsConnected())
        {
            DLDataRequestParameter LLCParameter = Parameters;
            if (m_MAC.DataRequest(AddLLCHeader(&LLCParameter)))
            {
                return m_MAC.Process();
            }
            return NOTHING_TO_DO;
        }
        return NOT_CONNECTED;
    }
    
    void HDLCClientLLC::RegisterDataIndication(CallbackFunction Callback)
    {
        m_MAC.RegisterCallback(DLDataRequestParameter::ID,
            std::bind(&HDLCClientLLC::MACDataIndication, this, std::placeholders::_1));
        RegisterCallback(DLDataRequestParameter::ID, Callback);

    }
    //
    // Transport
    //
    bool HDLCClientLLC::DataRequest(const Transport::DataRequestParameter& Parameters)
    {
        DLDataRequestParameter LLCParams(m_ConnectedAddress, HDLCControl::UI, Parameters.Data);
        return DataRequest(LLCParams);
    }
    
    bool HDLCClientLLC::MACDataIndication(const BaseCallbackParameter& Parameters)
    {
        bool RetVal = false;
        return FireCallback(DLDataRequestParameter::ID, Parameters, &RetVal) && RetVal;
    }

    //
    ///////////////////////////////////////////////////////////////////////////
    // 
    // Client MAC
    //
    ///////////////////////////////////////////////////////////////////////////
    //
    HDLCClient::HDLCClient(const HDLCAddress& MyAddress, 
        ISerial * pSerial, 
        const HDLCOptions& Opt,
        uint8_t MaxPreallocatedPacketBuffers) :
        HDLCMAC(MyAddress, pSerial, Opt, MaxPreallocatedPacketBuffers),
        StateMachine(ST_MAX_STATES)
    {
        //
        // State Machine
        //
        BEGIN_STATE_MAP
            STATE_MAP_ENTRY(ST_DISCONNECTED, HDLCClient::ST_Disconnected_Handler)
            STATE_MAP_ENTRY(ST_CONNECTING, HDLCClient::ST_Connecting_Handler)
            STATE_MAP_ENTRY(ST_CONNECTING_WAIT, HDLCClient::ST_Connecting_Wait_Handler)
            STATE_MAP_ENTRY(ST_CONNECTED, HDLCClient::ST_Connected_Handler)
            STATE_MAP_ENTRY(ST_CONNECTED_SEND, HDLCClient::ST_Connected_Send_Handler)
            STATE_MAP_ENTRY(ST_CONNECTED_RECEIVE, HDLCClient::ST_Connected_Receive_Handler)
        END_STATE_MAP
        //
        m_PacketCallback.RegisterCallback(HDLCControl::UI, 
            std::bind(&HDLCClient::UI_Handler, this, std::placeholders::_1));
        m_PacketCallback.RegisterCallback(HDLCControl::UA, 
            std::bind(&HDLCClient::UA_Handler, this, std::placeholders::_1));
    }
    
    HDLCClient::~HDLCClient()
    {
    }
    //
    // DA-CONNECT Service Implementation
    //
    bool HDLCClient::ConnectRequest(const DLConnectRequestOrIndication& Parameters)
    {
        bool bAllowed = false;
        BEGIN_TRANSITION_MAP
            TRANSITION_MAP_ENTRY(ST_DISCONNECTED, ST_CONNECTING)
            TRANSITION_MAP_ENTRY(ST_CONNECTING, EVENT_IGNORED)
            TRANSITION_MAP_ENTRY(ST_CONNECTING_WAIT, EVENT_IGNORED)
            TRANSITION_MAP_ENTRY(ST_CONNECTED, EVENT_IGNORED)
            TRANSITION_MAP_ENTRY(ST_CONNECTED_SEND, EVENT_IGNORED)
            TRANSITION_MAP_ENTRY(ST_CONNECTED_RECEIVE, EVENT_IGNORED)
        END_TRANSITION_MAP(bAllowed, new ConnectEventData(Parameters));
        return bAllowed;
    }
    //
    // DA-DATA Service Implementation
    //
    bool HDLCClient::DataRequest(const DLDataRequestParameter& Parameters)
    {
        bool bAllowed = false;
        BEGIN_TRANSITION_MAP
            TRANSITION_MAP_ENTRY(ST_DISCONNECTED, EVENT_IGNORED)
            TRANSITION_MAP_ENTRY(ST_CONNECTING, EVENT_IGNORED)
            TRANSITION_MAP_ENTRY(ST_CONNECTING_WAIT, EVENT_IGNORED)
            TRANSITION_MAP_ENTRY(ST_CONNECTED, ST_CONNECTED_SEND)
            TRANSITION_MAP_ENTRY(ST_CONNECTED_SEND, EVENT_IGNORED)
            TRANSITION_MAP_ENTRY(ST_CONNECTED_RECEIVE, EVENT_IGNORED)
        END_TRANSITION_MAP(bAllowed, new DataEventData(Parameters));
        return bAllowed;
    }
        
    HDLCRunResult HDLCClient::Process()
    {
        if (ProcessSerialTransmission() == SUCCESS)
        {
        }
        if (ProcessSerialReception() == SUCCESS)
        {
            ProcessPacketReception();
        }
        return RUN_WAIT;
    }

    void HDLCClient::ST_Disconnected_Handler(EventData * pData)
    {
    }
    
    void HDLCClient::ST_Connecting_Handler(EventData * pData)
    {
        ConnectEventData * pConnectData = dynamic_cast <ConnectEventData *>(pData);
        if (m_CurrentOptions.StartWithIEC)
        {
            ISerial::Options    IECOptions(ISerial::Options::BAUD_300, 7, ISerial::Options::PARITY_EVEN);
            if (m_pSerial->SetOptions(IECOptions) == SUCCESS &&
                m_pSerial->Write((const uint8_t *)("/?!\r\n"), 5) == SUCCESS)
            {
                // TODO - BETTER!
                uint8_t   Buffer[80] = { };
                uint8_t * pBuffer = Buffer;
                const uint8_t MODE_E_9600[] = { 0x06, 0x32, 0x35, 0x32, 0x0D, 0x0A };

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
                            ERROR_TYPE Result = m_pSerial->Read(pBuffer, 1, 2000);
                            if (SUCCESSFUL != Result)
                            {
                                CurrentState = STATE_BAUD;
                            }
                            else
                            {
                                if (pBuffer != Buffer && 
                                    *pBuffer == '\n' && (*(pBuffer - 1)) == '\r')
                                {
                                    CurrentState = STATE_BAUD;
                                    break;
                                }
                                ++pBuffer;
                            }
                        }
                        break;
                    case STATE_BAUD:
                        printf("CONNECT - %s\n",
                            Buffer);
                        m_pSerial->Write(MODE_E_9600,
                            sizeof(MODE_E_9600));
#ifdef __linux__
                        ::sleep(1);
#else
    #warning "Need to implement sleep for embedded!"
#endif
                        m_pSerial->SetOptions(
                            ISerial::Options(ISerial::Options::BAUD_9600, 8, 
                                ISerial::Options::PARITY_NONE, ISerial::Options::STOPBITS_ONE));
                        m_pSerial->Flush(ISerial::FlushDirection::BOTH);
                        CurrentState = STATE_HDLC;
                        break;
                        
                    default:
                        break;
                    }
                }
            }
        }
        
        Packet *           pSNRM = GetWorkingTXPacket();
        if (pSNRM)
        {
            HDLCErrorCode ReturnCode = pSNRM->MakePacket(Packet::NO_SEGMENT,
                pConnectData->Data.DestinationAddress,
                m_MyAddress,
                HDLCControl(HDLCControl::SNRM));
            if (SUCCESS == ReturnCode)
            {
                EnqueueWorkingTXPacket();
                InternalEvent(ST_CONNECTING_WAIT);
            }
            else
            {
                ReleaseWorkingTXPacket();
                InternalEvent(ST_DISCONNECTED);
            }
        }
    }
    
    void HDLCClient::ST_Connecting_Wait_Handler(EventData * pData)
    {
        // Start timer here...
    }
    
    void HDLCClient::ST_Connected_Handler(EventData * pData)
    {
        PacketEventData * pPacketData = dynamic_cast<PacketEventData *>(pData);
        if (nullptr != pData)
        {
            bool RetVal = false;
            FireCallback(DLConnectConfirmOrResponse::ID, 
                DLConnectConfirmOrResponse(pPacketData->Data.GetSourceAddress()),
                &RetVal);
            
            printf("CLIENT CONNECTED\n");
        }
    }
    
    void HDLCClient::ST_Connected_Send_Handler(EventData * pData)
    {
        DataEventData * pParam = dynamic_cast<DataEventData *>(pData);
        if (nullptr != pParam)
        {
            Packet *           pInfo = GetWorkingTXPacket();
            if (pInfo)
            {
                HDLCErrorCode ReturnCode = pInfo->MakePacket(Packet::NO_SEGMENT,
                    pParam->Data.DestinationAddress,
                    m_MyAddress,
                    HDLCControl(HDLCControl::INFO),
                    pParam->Data.Data.GetData(),
                    pParam->Data.Data.Size());
                if (SUCCESS == ReturnCode)
                {
                    EnqueueWorkingTXPacket();
                    //InternalEvent(ST_CONNECTED_RECEIVE);
                    InternalEvent(ST_CONNECTED);
                }
                else
                {
                    ReleaseWorkingTXPacket();
                    InternalEvent(ST_CONNECTED);
                }
            }
        }
    }
    
    void HDLCClient::ST_Connected_Receive_Handler(EventData * pData)
    {
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
            TRANSITION_MAP_ENTRY(ST_CONNECTING, EVENT_IGNORED)
            TRANSITION_MAP_ENTRY(ST_CONNECTING_WAIT, ST_CONNECTED)
            TRANSITION_MAP_ENTRY(ST_CONNECTED, EVENT_IGNORED)
            TRANSITION_MAP_ENTRY(ST_CONNECTED_SEND, EVENT_IGNORED)
            TRANSITION_MAP_ENTRY(ST_CONNECTED_RECEIVE, EVENT_IGNORED)
        END_TRANSITION_MAP(bAllowed, new PacketEventData(RXPacket));
        return bAllowed;
    }

}