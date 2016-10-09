#pragma once

#include <atomic>
#include <cstdint>
#include <cstddef>
#include <string>
#include <queue>
#include <map>
#include <functional>

#include "ERROR_TYPE.h"
#include "ISimpleTimer.h"
#include "modcnt.h"
#include "packet.h"
#include "HDLCControl.h"
#include "HDLCAddress.h"
#include "HDLCOptions.h"
#include "HDLCStatistics.h"
#include "StateMachine.h"
#include "Callback.h"

namespace EPRI
{
    enum HDLCRunResult : uint16_t
    {
        RUN_WAIT
    };

    class HDLCMAC : public Callback<HDLCErrorCode, uint16_t>
	{
    	using PacketCallback = Callback<HDLCErrorCode, HDLCControl::Control, Packet>;
    	
	public:
    	HDLCMAC() = delete;
    	HDLCMAC(const HDLCAddress& MyAddress, 
        	ISerial * pSerial, 
        	const HDLCOptions& Options,
        	uint8_t MaxPreallocatedPacketBuffers);
    	virtual ~HDLCMAC();
    	
    	virtual HDLCRunResult Process() = 0;

    	HDLCAddress MyAddress() const;
    	const HDLCStatistics& Statistics() const;
    	void ClearStatistics();
    	
	protected:
    	virtual HDLCErrorCode ProcessSerialReception();
    	Packet * GetWorkingRXPacket();
    	void ReleaseWorkingRXPacket();
    	void EnqueueWorkingRXPacket();            

    	virtual HDLCErrorCode ProcessSerialTransmission();
    	Packet * GetWorkingTXPacket();
    	void ReleaseWorkingTXPacket();
    	void EnqueueWorkingTXPacket();            
    	Packet * GetOutgoingPacket();
    	void ReleaseOutgoingPacket();
    	
    	virtual HDLCErrorCode ProcessPacketReception();
    	Packet * GetIncomingPacket();
    	void ReleaseIncomingPacket();
       	
    	HDLCAddress							m_MyAddress;
    	ISerial *                           m_pSerial;
    	HDLCOptions                         m_CurrentOptions;
		std::shared_ptr<EPRI::ISimpleTimer>	m_pTimer;
		HDLCStatistics					    m_Statistics;
    	std::queue<Packet>                  m_Packets;
    	std::queue<Packet>                  m_RXPackets;
    	std::queue<Packet>                  m_TXPackets;
    	PacketCallback                      m_PacketCallback;
    	
	private:
    	void LockPackets();
    	void UnlockPackets();
    	
    	std::atomic_flag                    m_PacketLock = ATOMIC_FLAG_INIT;
    	Packet *                            m_pRXPacket = nullptr;
    	Packet *                            m_pTXPacket = nullptr;
	};
    //
    // Base Callback Parameters
    //
    struct HDLCCallbackParameter : public BaseCallbackParameter
    {
        HDLCCallbackParameter(const HDLCAddress& DA)
            : DestinationAddress(DA)
        {
        }
        HDLCAddress DestinationAddress;
    };
    //
    // CONNECT
    //
    struct ConnectConfirmOrResponse : public HDLCCallbackParameter
    {
        static const uint16_t ID = __COUNTER__;
        ConnectConfirmOrResponse(const HDLCAddress& DA) :
            HDLCCallbackParameter(DA)
        {
        }
        // Result
        // User_Information
    };
    
    struct ConnectRequestOrIndication : public HDLCCallbackParameter
    {
        static const uint16_t ID = __COUNTER__;
        ConnectRequestOrIndication(const HDLCAddress& DA) :
            HDLCCallbackParameter(DA)
        {
        }
        // User_Information
    };
    //
    // DATA
    //
    struct DataRequestParameter : public HDLCCallbackParameter
    {
        static const uint16_t ID = __COUNTER__;
        DataRequestParameter(const HDLCAddress& DA, HDLCControl::Control FT, const std::vector<uint8_t>& D)  :
            HDLCCallbackParameter(DA),
            FrameType(FT),
            Data(D)
        {
        }
        HDLCControl::Control FrameType;
        std::vector<uint8_t> Data;
    };

    template<typename TInternal>
    class MACEventData : public EventData
    {
    public:
        MACEventData(const TInternal& D)
            : Data(D)
        {
        }

        virtual void Release()
        {
            delete this;
        }
        
        TInternal Data;
        
    };
    
    using ConnectEventData = MACEventData<ConnectRequestOrIndication>;
    using ConnectResponseData = MACEventData<ConnectConfirmOrResponse>;
    using PacketEventData = MACEventData<Packet>;
    using DataEventData = MACEventData<DataRequestParameter>;
    
    class HDLCClient : public HDLCMAC, public StateMachine
    {
    public:
        HDLCClient() = delete;
        HDLCClient(const HDLCAddress& MyAddress, 
            ISerial * pSerial, 
            const HDLCOptions& Opt,
            uint8_t MaxPreallocatedPacketBuffers);
        virtual ~HDLCClient();
        //
        // MA-CONNECT Service
        //
        HDLCErrorCode ConnectRequest(const ConnectRequestOrIndication& Parameters);
        //
        // MA-CONNECT Service
        //
        HDLCErrorCode DataRequest(const DataRequestParameter& Parameters);

        HDLCRunResult Process();
        
    private:
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
            
        enum States : uint8_t
        {
            ST_DISCONNECTED = 0,
            ST_CONNECTING,
            ST_CONNECTING_WAIT,
            ST_CONNECTED,
            ST_CONNECTED_SEND,
            ST_CONNECTED_RECEIVE,
            ST_MAX_STATES
        };

        void ST_Disconnected_Handler(EventData * pData);
        void ST_Connecting_Handler(EventData * pData);
        void ST_Connecting_Wait_Handler(EventData * pData);
        void ST_Connected_Handler(EventData * pData);
        void ST_Connected_Send_Handler(EventData * pData);
        void ST_Connected_Receive_Handler(EventData * pData);
        //
        // Packet Handlers
        //
        HDLCErrorCode UI_Handler(const Packet& RXPacket);
        HDLCErrorCode UA_Handler(const Packet& RXPacket);
       
    };
    
    class HDLCServer : public HDLCMAC, public StateMachine
    {
    public:
        HDLCServer() = delete;
        HDLCServer(const HDLCAddress& MyAddress, 
            ISerial * pSerial, 
            const HDLCOptions& Opt,
            uint8_t MaxPreallocatedPacketBuffers);
        virtual ~HDLCServer();
        //
        // MA-CONNECT Service
        //
        HDLCErrorCode ConnectResponse(const ConnectConfirmOrResponse& Parameters);
        //
        // MA-CONNECT Service
        //
        HDLCErrorCode DataRequest(const DataRequestParameter& Parameters);

        HDLCRunResult Process();
        
    private:
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
            
        enum States : uint8_t
        {
            ST_DISCONNECTED = 0,
            ST_CONNECTING,
            ST_CONNECTING_RESPONSE,
            ST_CONNECTED,
            ST_CONNECTED_SEND,
            ST_CONNECTED_RECEIVE,
            ST_MAX_STATES
        };
        
        void ST_Disconnected_Handler(EventData * pData);
        void ST_Connecting_Handler(EventData * pData);
        void ST_Connecting_Response_Handler(EventData * pData);
        void ST_Connected_Handler(EventData * pData);
        void ST_Connected_Send_Handler(EventData * pData);
        void ST_Connected_Receive_Handler(EventData * pData);
        //
        // Packet Handlers
        //
        HDLCErrorCode SNRM_Handler(const Packet& Packet);
        HDLCErrorCode I_Handler(const Packet& RXPacket);
        
    };


} /* namespace EPRI */

