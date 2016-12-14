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
        m_MAC.RegisterCallback(DLConnectRequestOrIndication::ID,
            std::bind(&HDLCServerLLC::MACConnectIndication, this, std::placeholders::_1));
        m_MAC.RegisterCallback(DLDataRequestParameter::ID,
            std::bind(&HDLCServerLLC::MACDataIndication, this, std::placeholders::_1));
    }
    
    HDLCServerLLC::~HDLCServerLLC()
    {
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
            TRANSITION_MAP_ENTRY(ST_IEC_CONNECT, EVENT_IGNORED)
            TRANSITION_MAP_ENTRY(ST_CONNECTING_WAIT, ST_CONNECTED)
            TRANSITION_MAP_ENTRY(ST_CONNECTED, EVENT_IGNORED)
        END_TRANSITION_MAP(bAllowed, new ConnectResponseData(Parameters));
        return bAllowed;
    }
       
    void HDLCServer::Process()
    {
        Base()->GetScheduler()->Post(std::bind(&HDLCServer::ProcessSerialTransmission, this));
    }
    
    void HDLCServer::ST_Disconnected_Handler(EventData * pData)
    {
        Process();
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

}