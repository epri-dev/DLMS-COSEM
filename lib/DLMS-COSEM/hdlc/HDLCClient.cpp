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
        }
        return RetVal;
    }
    //
    // DA-DATA Service Implementation
    //
    HDLCRunResult HDLCClientLLC::DataRequest(const DLDataRequestParameter& Parameters)
    {
        if (m_MAC.DataRequest(Parameters))
        {
            return m_MAC.Process();
        }
        return NOTHING_TO_DO;
    }
    
    void HDLCClientLLC::RegisterDataIndication(CallbackFunction Callback)
    {
        m_MAC.RegisterCallback(DLDataRequestParameter::ID,
            std::bind(&HDLCClientLLC::MACDataIndication, this, std::placeholders::_1));
        RegisterCallback(DLDataRequestParameter::ID, Callback);

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
                DLConnectConfirmOrResponse(pPacketData->Data.GetDestinationAddress()),
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
                    pParam->Data.Data.data(),
                    pParam->Data.Data.size());
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