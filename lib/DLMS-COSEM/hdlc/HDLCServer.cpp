#include "hdlc/HDLCMAC.h"
#include "HDLCLLC.h"

namespace EPRI
{
    ///////////////////////////////////////////////////////////////////////////
    // 
    // Server LLC
    //
    ///////////////////////////////////////////////////////////////////////////
    //
    HDLCServerLLC::HDLCServerLLC(const HDLCAddress& MyAddress, 
        ISerial * pSerial, 
        const HDLCOptions& Options,
        uint8_t MaxPreallocatedPacketBuffers) :
        HDLCLLC(&m_MAC), 
        m_MAC(MyAddress, pSerial, Options, MaxPreallocatedPacketBuffers)
    {
    }
    
    HDLCServerLLC::~HDLCServerLLC()
    {
    }
    //
    // DA-CONNECT Service
    //
    void HDLCServerLLC::RegisterConnectIndication(CallbackFunction Callback)
    {
        m_MAC.RegisterCallback(DLConnectRequestOrIndication::ID,
            std::bind(&HDLCServerLLC::MACConnectIndication, this, std::placeholders::_1));
        RegisterCallback(DLConnectRequestOrIndication::ID, Callback);
    }
    
    HDLCRunResult HDLCServerLLC::ConnectResponse(const DLConnectConfirmOrResponse& Parameters)
    {
        if (m_MAC.ConnectResponse(Parameters))
        {
            return m_MAC.Process();
        }
        return NOTHING_TO_DO;
    }

    bool HDLCServerLLC::MACConnectIndication(const BaseCallbackParameter& Parameters)
    {
        bool RetVal = false;
        if (FireCallback(DLConnectRequestOrIndication::ID, Parameters, &RetVal) && RetVal)
        {
            FireTransportEvent(Transport::TRANSPORT_CONNECTED);
        }
        return RetVal;
    }
    //
    // DA-DATA Service
    //
    HDLCRunResult HDLCServerLLC::DataRequest(const DLDataRequestParameter& Parameters)
    {
        if (m_MAC.DataRequest(Parameters))
        {
            return m_MAC.Process();
        }
        return NOTHING_TO_DO;
    }

    void HDLCServerLLC::RegisterDataIndication(CallbackFunction Callback)
    {
        m_MAC.RegisterCallback(DLDataRequestParameter::ID,
            std::bind(&HDLCServerLLC::MACDataIndication, this, std::placeholders::_1));
        RegisterCallback(DLDataRequestParameter::ID, Callback);
    }
    //
    // Transport
    //
    bool HDLCServerLLC::DataRequest(const Transport::DataRequestParameter& Parameters)
    {
        return false;
    }
    
    bool HDLCServerLLC::MACDataIndication(const BaseCallbackParameter& Parameters)
    {
        bool RetVal = false;
        return FireCallback(DLDataRequestParameter::ID, Parameters, &RetVal) && RetVal;
    }

    //
    ///////////////////////////////////////////////////////////////////////////
    // 
    // Server MAC
    //
    ///////////////////////////////////////////////////////////////////////////
    //
    HDLCServer::HDLCServer(const HDLCAddress& MyAddress, 
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
            STATE_MAP_ENTRY(ST_DISCONNECTED, HDLCServer::ST_Disconnected_Handler)
            STATE_MAP_ENTRY(ST_CONNECTING, HDLCServer::ST_Connecting_Handler)
            STATE_MAP_ENTRY(ST_CONNECTING_RESPONSE, HDLCServer::ST_Connecting_Response_Handler)
            STATE_MAP_ENTRY(ST_CONNECTED, HDLCServer::ST_Connected_Handler)
            STATE_MAP_ENTRY(ST_CONNECTED_SEND, HDLCServer::ST_Connected_Send_Handler)
            STATE_MAP_ENTRY(ST_CONNECTED_RECEIVE, HDLCServer::ST_Connected_Receive_Handler)
        END_STATE_MAP
        //    
        m_PacketCallback.RegisterCallback(HDLCControl::SNRM, 
            std::bind(&HDLCServer::SNRM_Handler, this, std::placeholders::_1));
        m_PacketCallback.RegisterCallback(HDLCControl::INFO, 
            std::bind(&HDLCServer::I_Handler, this, std::placeholders::_1));
    }
    
    HDLCServer::~HDLCServer()
    {
    }
    //
    // DA-CONNECT Service
    //
    bool HDLCServer::ConnectResponse(const DLConnectConfirmOrResponse& Parameters)
    {
        bool bAllowed = false;
        BEGIN_TRANSITION_MAP
            TRANSITION_MAP_ENTRY(ST_DISCONNECTED, EVENT_IGNORED)
            TRANSITION_MAP_ENTRY(ST_CONNECTING, ST_CONNECTING_RESPONSE)
            TRANSITION_MAP_ENTRY(ST_CONNECTING_RESPONSE, EVENT_IGNORED)
            TRANSITION_MAP_ENTRY(ST_CONNECTED, EVENT_IGNORED)
            TRANSITION_MAP_ENTRY(ST_CONNECTED_SEND, EVENT_IGNORED)
            TRANSITION_MAP_ENTRY(ST_CONNECTED_RECEIVE, EVENT_IGNORED)
        END_TRANSITION_MAP(bAllowed, new ConnectResponseData(Parameters));
        return bAllowed;
    }
    //
    // DA-DATA Service Implementation
    //
    bool HDLCServer::DataRequest(const DLDataRequestParameter& Parameters)
    {
    }
       
    HDLCRunResult HDLCServer::Process()
    {
        if (ProcessSerialReception() == SUCCESS)
        {
            ProcessPacketReception();
        }
        ProcessSerialTransmission();
        return RUN_WAIT;
    }
    
    void HDLCServer::ST_Disconnected_Handler(EventData * pData)
    {
        printf("SERVER DISCONNECTED\n");
    }
    
    void HDLCServer::ST_Connected_Handler(EventData * pData)
    {
        printf("SERVER CONNECTED\n");
    }
    
    void HDLCServer::ST_Connecting_Response_Handler(EventData * pData)
    {
        ConnectResponseData * pResponseData = dynamic_cast<ConnectResponseData *>(pData);
        if (nullptr != pData)
        {
            Packet *      pUA = GetWorkingTXPacket();
            if (pUA)
            {
                HDLCErrorCode ReturnCode = pUA->MakePacket(Packet::NO_SEGMENT,
                    pResponseData->Data.DestinationAddress,
                    m_MyAddress,
                    HDLCControl(HDLCControl::UA));
                if (SUCCESS == ReturnCode)
                {
                    EnqueueWorkingTXPacket();
                    InternalEvent(ST_CONNECTED);
                }
                else
                {
                    ReleaseWorkingTXPacket();
                    InternalEvent(ST_DISCONNECTED);
                }
            }
        }
    }
    
    void HDLCServer::ST_Connecting_Handler(EventData * pData)
    {
        PacketEventData * pPacketData = dynamic_cast<PacketEventData *>(pData);
        if (nullptr != pData)
        {
            bool RetVal = false;
            DLConnectRequestOrIndication Params(pPacketData->Data.GetSourceAddress());
            RetVal = FireCallback(DLConnectRequestOrIndication::ID, 
                Params,
                &RetVal) && RetVal;
            if (!RetVal)
            {
                Packet *      pDM = GetWorkingTXPacket();
                if (pDM)
                {
                    HDLCErrorCode ReturnCode = pDM->MakePacket(Packet::NO_SEGMENT,
                        pPacketData->Data.GetSourceAddress(),
                        m_MyAddress,
                        HDLCControl(HDLCControl::DM));
                    if (SUCCESS == ReturnCode)
                    {
                        EnqueueWorkingTXPacket();
                    }
                    else
                    {
                        ReleaseWorkingTXPacket();
                    }
                    InternalEvent(ST_DISCONNECTED);
                }
            }
        }
    }
    
    void HDLCServer::ST_Connected_Send_Handler(EventData * pData)
    {
    }
    
    void HDLCServer::ST_Connected_Receive_Handler(EventData * pData)
    {
        PacketEventData * pPacketData = dynamic_cast<PacketEventData *>(pData);
        if (nullptr != pData)
        {
            bool            RetVal = false;
            size_t          InfoLength = 0;
            const uint8_t * pInformation = pPacketData->Data.GetInformation(InfoLength);
            
            FireCallback(DLDataRequestParameter::ID, 
                DLDataRequestParameter(pPacketData->Data.GetSourceAddress(),
                    HDLCControl::INFO,
                    std::vector<uint8_t>(pInformation, pInformation + InfoLength)),
                &RetVal);

            InternalEvent(ST_CONNECTED);
        }

    }
    //
    // Packet Handlers
    //
    bool HDLCServer::SNRM_Handler(const Packet& RXPacket)
    {
        bool bAllowed = false;
        BEGIN_TRANSITION_MAP
            TRANSITION_MAP_ENTRY(ST_DISCONNECTED, ST_CONNECTING)
            TRANSITION_MAP_ENTRY(ST_CONNECTING, EVENT_IGNORED)
            TRANSITION_MAP_ENTRY(ST_CONNECTING_RESPONSE, EVENT_IGNORED)
            TRANSITION_MAP_ENTRY(ST_CONNECTED, EVENT_IGNORED)
            TRANSITION_MAP_ENTRY(ST_CONNECTED_SEND, EVENT_IGNORED)
            TRANSITION_MAP_ENTRY(ST_CONNECTED_RECEIVE, EVENT_IGNORED)
        END_TRANSITION_MAP(bAllowed, new PacketEventData(RXPacket));
        return bAllowed;
    }
    
    bool HDLCServer::I_Handler(const Packet& RXPacket)
    {
        bool bAllowed = false;
        BEGIN_TRANSITION_MAP
            TRANSITION_MAP_ENTRY(ST_DISCONNECTED, EVENT_IGNORED)
            TRANSITION_MAP_ENTRY(ST_CONNECTING, EVENT_IGNORED)
            TRANSITION_MAP_ENTRY(ST_CONNECTING_RESPONSE, EVENT_IGNORED)
            TRANSITION_MAP_ENTRY(ST_CONNECTED, ST_CONNECTED_RECEIVE)
            TRANSITION_MAP_ENTRY(ST_CONNECTED_SEND, EVENT_IGNORED)
            TRANSITION_MAP_ENTRY(ST_CONNECTED_RECEIVE, ST_CONNECTED_RECEIVE)
        END_TRANSITION_MAP(bAllowed, new PacketEventData(RXPacket));
        return bAllowed;
    }

}