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
        m_MAC.RegisterCallback(ConnectRequestOrIndication::ID,
            std::bind(&HDLCServerLLC::MACConnectIndication, this, std::placeholders::_1));
        RegisterCallback(ConnectRequestOrIndication::ID, Callback);
    }
    
    HDLCRunResult HDLCServerLLC::ConnectResponse(const ConnectConfirmOrResponse& Parameters)
    {
        HDLCErrorCode RetVal;
        switch ((RetVal = m_MAC.ConnectResponse(Parameters)))
        {
        default:
            break;
        }
        return m_MAC.Process();
    }

    HDLCErrorCode HDLCServerLLC::MACConnectIndication(const BaseCallbackParameter& Parameters)
    {
        HDLCErrorCode RetVal = FAIL;
        FireCallback(ConnectRequestOrIndication::ID, Parameters, &RetVal);
        return RetVal;
    }
    //
    // DA-DATA Service
    //
    HDLCRunResult HDLCServerLLC::DataRequest(const DataRequestParameter& Parameters)
    {
        HDLCErrorCode RetVal;
        switch ((RetVal = m_MAC.DataRequest(Parameters)))
        {
        default:
            break;
        }
        return m_MAC.Process();
    }

    void HDLCServerLLC::RegisterDataIndication(CallbackFunction Callback)
    {
        m_MAC.RegisterCallback(DataRequestParameter::ID,
            std::bind(&HDLCServerLLC::MACDataIndication, this, std::placeholders::_1));
        RegisterCallback(DataRequestParameter::ID, Callback);
    }
    
    HDLCErrorCode HDLCServerLLC::MACDataIndication(const BaseCallbackParameter& Parameters)
    {
        HDLCErrorCode RetVal = FAIL;
        FireCallback(DataRequestParameter::ID, Parameters, &RetVal);
        return RetVal;
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
    HDLCErrorCode HDLCServer::ConnectResponse(const ConnectConfirmOrResponse& Parameters)
    {
        BEGIN_TRANSITION_MAP
            TRANSITION_MAP_ENTRY(ST_DISCONNECTED, EVENT_IGNORED)
            TRANSITION_MAP_ENTRY(ST_CONNECTING, ST_CONNECTING_RESPONSE)
            TRANSITION_MAP_ENTRY(ST_CONNECTING_RESPONSE, EVENT_IGNORED)
            TRANSITION_MAP_ENTRY(ST_CONNECTED, EVENT_IGNORED)
            TRANSITION_MAP_ENTRY(ST_CONNECTED_SEND, EVENT_IGNORED)
            TRANSITION_MAP_ENTRY(ST_CONNECTED_RECEIVE, EVENT_IGNORED)
        END_TRANSITION_MAP(new ConnectResponseData(Parameters));
        return SUCCESS;
    }
    //
    // DA-DATA Service Implementation
    //
    HDLCErrorCode HDLCServer::DataRequest(const DataRequestParameter& Parameters)
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
            HDLCErrorCode RetVal = FAIL;
            ConnectRequestOrIndication Params(pPacketData->Data.GetSourceAddress());
            
            FireCallback(ConnectRequestOrIndication::ID, 
                Params,
                &RetVal);
            if (RetVal != SUCCESS)
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
            HDLCErrorCode   RetVal = SUCCESS;
            size_t          InfoLength = 0;
            const uint8_t * pInformation = pPacketData->Data.GetInformation(InfoLength);
            FireCallback(DataRequestParameter::ID, 
                DataRequestParameter(pPacketData->Data.GetSourceAddress(),
                    HDLCControl::INFO,
                    std::vector<uint8_t>(pInformation, pInformation + InfoLength)),
                &RetVal);

            InternalEvent(ST_CONNECTED);
        }

    }
    //
    // Packet Handlers
    //
    HDLCErrorCode HDLCServer::SNRM_Handler(const Packet& RXPacket)
    {
        BEGIN_TRANSITION_MAP
            TRANSITION_MAP_ENTRY(ST_DISCONNECTED, ST_CONNECTING)
            TRANSITION_MAP_ENTRY(ST_CONNECTING, EVENT_IGNORED)
            TRANSITION_MAP_ENTRY(ST_CONNECTING_RESPONSE, EVENT_IGNORED)
            TRANSITION_MAP_ENTRY(ST_CONNECTED, EVENT_IGNORED)
            TRANSITION_MAP_ENTRY(ST_CONNECTED_SEND, EVENT_IGNORED)
            TRANSITION_MAP_ENTRY(ST_CONNECTED_RECEIVE, EVENT_IGNORED)
        END_TRANSITION_MAP(new PacketEventData(RXPacket));
        return SUCCESS;
    }
    
    HDLCErrorCode HDLCServer::I_Handler(const Packet& RXPacket)
    {
        BEGIN_TRANSITION_MAP
            TRANSITION_MAP_ENTRY(ST_DISCONNECTED, EVENT_IGNORED)
            TRANSITION_MAP_ENTRY(ST_CONNECTING, EVENT_IGNORED)
            TRANSITION_MAP_ENTRY(ST_CONNECTING_RESPONSE, EVENT_IGNORED)
            TRANSITION_MAP_ENTRY(ST_CONNECTED, ST_CONNECTED_RECEIVE)
            TRANSITION_MAP_ENTRY(ST_CONNECTED_SEND, EVENT_IGNORED)
            TRANSITION_MAP_ENTRY(ST_CONNECTED_RECEIVE, ST_CONNECTED_RECEIVE)
        END_TRANSITION_MAP(new PacketEventData(RXPacket));
        return SUCCESS;
    }


}