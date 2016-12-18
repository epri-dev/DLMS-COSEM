#include <functional>

#include "IBaseLibrary.h"
#include "IScheduler.h"
#include "IDebug.h"
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
        ISerialSocket * pSerial, 
        const HDLCOptions& Options,
        uint8_t MaxPreallocatedPacketBuffers) :
        HDLCLLC(&m_MAC), 
        m_MAC(MyAddress, pSerial, Options, MaxPreallocatedPacketBuffers)
    {
        m_MAC.RegisterCallback(DLIdentifyRequestParameter::ID,
            std::bind(&HDLCServerLLC::MACIdentifyIndication, this, std::placeholders::_1));
        m_MAC.RegisterCallback(DLConnectRequestOrIndication::ID,
            std::bind(&HDLCServerLLC::MACConnectIndication, this, std::placeholders::_1));
        m_MAC.RegisterCallback(DLDataRequestParameter::ID,
            std::bind(&HDLCServerLLC::MACDataIndication, this, std::placeholders::_1));
        m_MAC.RegisterCallback(DLDisconnectRequestOrIndication::ID,
            std::bind(&HDLCServerLLC::MACDisconnectIndication, this, std::placeholders::_1));
    }
    
    HDLCServerLLC::~HDLCServerLLC()
    {
    }
    //
    // IDENTIFY Service
    //
    bool HDLCServerLLC::IdentifyResponse(const DLIdentifyResponseParameter& Parameters)
    {
        return m_MAC.IdentifyResponse(Parameters);
    }

    bool HDLCServerLLC::MACIdentifyIndication(const BaseCallbackParameter& Parameters)
    {
        const DLIdentifyRequestParameter& IdentifyParams = 
            dynamic_cast<const DLIdentifyRequestParameter&>(Parameters);
        
        return IdentifyResponse(DLIdentifyResponseParameter(IdentifyParams.DestinationAddress));
    }
    //
    // DA-CONNECT Service
    //
    bool HDLCServerLLC::ConnectResponse(const DLConnectConfirmOrResponse& Parameters)
    {
        return m_MAC.ConnectResponse(Parameters);
    }

    bool HDLCServerLLC::MACConnectIndication(const BaseCallbackParameter& Parameters)
    {
        const DLConnectRequestOrIndication& ConnectParams = 
            dynamic_cast<const DLConnectRequestOrIndication&>(Parameters);
        bool RetVal = OnConnectIndication(ConnectParams.DestinationAddress.LogicalAddress());
        if (RetVal)
        {
            RetVal = MACConnectConfirmOrIndication(Parameters);
        }
        return RetVal;
    }
    
    bool HDLCServerLLC::OnConnectIndication(COSEMAddressType Address)
    {
        DLConnectConfirmOrResponse Response(Address);
        ConnectResponse(Response);
        return true;
    }
    //
    // DA-DISCONNECT Service
    //
    bool HDLCServerLLC::DisconnectResponse(const DLDisconnectConfirmOrResponse& Parameters)
    {
        return m_MAC.DisconnectResponse(Parameters);
    }

    bool HDLCServerLLC::MACDisconnectIndication(const BaseCallbackParameter& Parameters)
    {
        const DLDisconnectRequestOrIndication& DisconnectParams = 
            dynamic_cast<const DLDisconnectRequestOrIndication&>(Parameters);
        bool RetVal = OnDisconnectIndication(DisconnectParams.DestinationAddress.LogicalAddress());
        if (RetVal)
        {
            RetVal = MACDisconnectConfirmOrIndication(Parameters);
        }
        return RetVal;
    }
    
    bool HDLCServerLLC::OnDisconnectIndication(COSEMAddressType Address)
    {
        DLDisconnectConfirmOrResponse Response(Address);
        DisconnectResponse(Response);
        return true;
    }    
    //
    ///////////////////////////////////////////////////////////////////////////
    // 
    // Server MAC
    //
    ///////////////////////////////////////////////////////////////////////////
    //
    HDLCServer::HDLCServer(const HDLCAddress& MyAddress, 
        ISerialSocket * pSerial, 
        const HDLCOptions& Opt,
        uint8_t MaxPreallocatedPacketBuffers) : 
        HDLCMAC(MyAddress, pSerial, Opt, MaxPreallocatedPacketBuffers)
    {
        m_PacketCallback.RegisterCallback(HDLCControl::SNRM, 
            std::bind(&HDLCServer::SNRM_Handler, this, std::placeholders::_1));
        m_PacketCallback.RegisterCallback(HDLCControl::IDENT, 
            std::bind(&HDLCServer::IDENT_Handler, this, std::placeholders::_1));
        m_PacketCallback.RegisterCallback(HDLCControl::DISC, 
            std::bind(&HDLCServer::DISC_Handler, this, std::placeholders::_1));
    }
    
    HDLCServer::~HDLCServer()
    {
    }
    //
    // IDENTIFY Service
    //
    bool HDLCServer::IdentifyResponse(const DLIdentifyResponseParameter& Parameters)
    {
        bool bAllowed = false;
        BEGIN_TRANSITION_MAP
            TRANSITION_MAP_ENTRY(ST_DISCONNECTED, ST_DISCONNECTED)
            TRANSITION_MAP_ENTRY(ST_IEC_CONNECT, EVENT_IGNORED)
            TRANSITION_MAP_ENTRY(ST_CONNECTING_WAIT, EVENT_IGNORED)
            TRANSITION_MAP_ENTRY(ST_CONNECTED, EVENT_IGNORED)
        END_TRANSITION_MAP(bAllowed, new IdentifyResponseData(Parameters));
        return bAllowed;
    }
    //
    // DA-CONNECT Service
    //
    bool HDLCServer::ConnectResponse(const DLConnectConfirmOrResponse& Parameters)
    {
        bool bAllowed = false;
        BEGIN_TRANSITION_MAP
            TRANSITION_MAP_ENTRY(ST_DISCONNECTED, EVENT_IGNORED)
            TRANSITION_MAP_ENTRY(ST_IEC_CONNECT, EVENT_IGNORED)
            TRANSITION_MAP_ENTRY(ST_CONNECTING_WAIT, ST_CONNECTED)
            TRANSITION_MAP_ENTRY(ST_CONNECTED, EVENT_IGNORED)
        END_TRANSITION_MAP(bAllowed, new ConnectResponseData(Parameters));
        return bAllowed;
    }
    //
    // DA-DISCONNECT Service
    //
    bool HDLCServer::DisconnectResponse(const DLDisconnectConfirmOrResponse& Parameters)
    {
        bool bAllowed = false;
        BEGIN_TRANSITION_MAP
            TRANSITION_MAP_ENTRY(ST_DISCONNECTED, ST_DISCONNECTED)
            TRANSITION_MAP_ENTRY(ST_IEC_CONNECT, ST_DISCONNECTED)
            TRANSITION_MAP_ENTRY(ST_CONNECTING_WAIT, ST_DISCONNECTED)
            TRANSITION_MAP_ENTRY(ST_CONNECTED, ST_DISCONNECTED)
        END_TRANSITION_MAP(bAllowed, new DisconnectResponseData(Parameters));
        return bAllowed;
    }
       
    void HDLCServer::Process()
    {
        Base()->GetScheduler()->Post(std::bind(&HDLCServer::ProcessSerialTransmission, this));
    }
    
    void HDLCServer::ST_Disconnected_Handler(EventData * pData)
    {
        //
        // Disconnected State Entry
        //
        m_ConnectedAddress.Clear();
        //
        // DISC - Disconnect Request
        //
        PacketEventData * pPacketData = dynamic_cast<PacketEventData *>(pData);
        if (pPacketData && pPacketData->Data.GetControl().PacketType() == HDLCControl::DISC)
        {
            bool RetVal = false;
            RetVal = FireCallback(DLDisconnectRequestOrIndication::ID, 
                DLDisconnectRequestOrIndication(pPacketData->Data.GetSourceAddress()),
                &RetVal) && RetVal;

            Process();
            return;
        }            
        //
        // DISCONNECT Response
        //
        DisconnectResponseData * pDisconnectData = dynamic_cast<DisconnectResponseData *>(pData);
        if (pDisconnectData)
        {
            Packet *      pDM = GetWorkingTXPacket();
            if (pDM)
            {
                DLDisconnectConfirmOrResponse& Data = pDisconnectData->Data;
                HDLCErrorCode ReturnCode = pDM->MakePacket(Packet::NO_SEGMENT,
                    pDisconnectData->Data.DestinationAddress,
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
            }
            Process();
            return;
        }           
        //
        // IDENT
        //
        pPacketData = dynamic_cast<PacketEventData *>(pData);
        if (pPacketData && pPacketData->Data.GetControl().PacketType() == HDLCControl::IDENT)
        {
            bool RetVal = false;
            RetVal = FireCallback(DLIdentifyRequestParameter::ID, 
                DLIdentifyRequestParameter(m_MyAddress),
                &RetVal) && RetVal;

            Process();
            return;
        }    
        //
        // IDENTIFY Response
        //
        IdentifyResponseData * pIdentifyData = dynamic_cast<IdentifyResponseData *>(pData);
        if (pIdentifyData)
        {
            Packet *      pIDENTR = GetWorkingTXPacket();
            if (pIDENTR)
            {
                DLIdentifyResponseParameter& Data = pIdentifyData->Data;
                HDLCErrorCode                ReturnCode = 
                    pIDENTR->MakeIdentifyPacket(HDLCControl(HDLCControl::IDENTR),
                                                Data.SuccessCode, Data.ProtocolID,
                                                Data.ProtocolVersion, Data.ProtocolRevision);
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
    }
    
    void HDLCServer::ST_IEC_Connect_Handler(EventData * pData)
    {
    }
    
    void HDLCServer::ST_Connecting_Wait_Handler(EventData * pData)
    {
        //
        // SNRM
        //
        PacketEventData * pPacketData = dynamic_cast<PacketEventData *>(pData);
        if (pPacketData && pPacketData->Data.GetControl().PacketType() == HDLCControl::SNRM)
        {
            bool RetVal = false;
            DLConnectRequestOrIndication Params(pPacketData->Data.GetSourceAddress());
            RetVal = FireCallback(DLConnectRequestOrIndication::ID, 
                Params,
                &RetVal) && RetVal;
            if (!RetVal)
            {
                InternalEvent(ST_DISCONNECTED,
                    new DisconnectResponseData(DLDisconnectConfirmOrResponse(pPacketData->Data.GetSourceAddress())));
            }
            Process();
            return;
        }
        
    }
    
    void HDLCServer::ST_Connected_Handler(EventData * pData)
    {
        //
        // Connect Response
        //
        ConnectResponseData * pResponseData = dynamic_cast<ConnectResponseData *>(pData);
        if (pResponseData)
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
                    m_ConnectedAddress = pResponseData->Data.DestinationAddress;
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
        //
        // Default Handler - Handles Data
        //
        HDLCMAC::ST_Connected_Handler(pData);

        Process();
        
    }
    //
    // Packet Handlers
    //
    bool HDLCServer::SNRM_Handler(const Packet& RXPacket)
    {
        bool bAllowed = false;
        BEGIN_TRANSITION_MAP
            TRANSITION_MAP_ENTRY(ST_DISCONNECTED, ST_CONNECTING_WAIT)
            TRANSITION_MAP_ENTRY(ST_IEC_CONNECT, EVENT_IGNORED)
            TRANSITION_MAP_ENTRY(ST_CONNECTING_WAIT, EVENT_IGNORED)
            TRANSITION_MAP_ENTRY(ST_CONNECTED, EVENT_IGNORED)
        END_TRANSITION_MAP(bAllowed, new PacketEventData(RXPacket));
        return bAllowed;
    }
    
    bool HDLCServer::IDENT_Handler(const Packet& RXPacket)
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
    
    bool HDLCServer::DISC_Handler(const Packet& RXPacket)
    {
        bool bAllowed = false;
        BEGIN_TRANSITION_MAP
            TRANSITION_MAP_ENTRY(ST_DISCONNECTED, ST_DISCONNECTED)
            TRANSITION_MAP_ENTRY(ST_IEC_CONNECT, ST_DISCONNECTED)
            TRANSITION_MAP_ENTRY(ST_CONNECTING_WAIT, ST_DISCONNECTED)
            TRANSITION_MAP_ENTRY(ST_CONNECTED, ST_DISCONNECTED)
        END_TRANSITION_MAP(bAllowed, new PacketEventData(RXPacket));
        return bAllowed;
    }
}