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
#include "DLMSVector.h"

namespace EPRI
{
    enum HDLCRunResult : uint16_t
    {
        RUN_WAIT,
        NOTHING_TO_DO,
        NOT_CONNECTED
    };

    class HDLCMAC : public Callback<bool, uint16_t>
	{
    	using PacketCallback = Callback<bool, HDLCControl::Control, Packet>;
    	
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
    	using PacketPtr = std::unique_ptr<Packet>;

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
    	std::queue<PacketPtr>               m_Packets;
    	std::queue<PacketPtr>               m_RXPackets;
    	std::queue<PacketPtr>               m_TXPackets;
    	PacketCallback                      m_PacketCallback;
    	
	private:
    	void LockPackets();
    	void UnlockPackets();
    	
    	std::atomic_flag                    m_PacketLock = ATOMIC_FLAG_INIT;
    	PacketPtr                           m_pRXPacket = nullptr;
    	PacketPtr                           m_pTXPacket = nullptr;
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
    struct DLConnectConfirmOrResponse : public HDLCCallbackParameter
    {
        static const uint16_t ID = 0x1001;
        DLConnectConfirmOrResponse(const HDLCAddress& DA)
            :
            HDLCCallbackParameter(DA)
        {
        }
        // Result
        // User_Information
    };
    
    struct DLConnectRequestOrIndication : public HDLCCallbackParameter
    {
        static const uint16_t ID = 0x1002;
        DLConnectRequestOrIndication(const HDLCAddress& DA)
            :
            HDLCCallbackParameter(DA)
        {
        }
        // User_Information
    };
    //
    // DATA
    //
    struct DLDataRequestParameter : public HDLCCallbackParameter
    {
        static const uint16_t ID = 0x1004;
        DLDataRequestParameter(const HDLCAddress& DA, HDLCControl::Control FT, const DLMSVector& D)
            :
            HDLCCallbackParameter(DA),
            FrameType(FT),
            Data(D)
        {
        }
        HDLCControl::Control FrameType;
        DLMSVector           Data;
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
    
    using ConnectEventData = MACEventData<DLConnectRequestOrIndication>;
    using ConnectResponseData = MACEventData<DLConnectConfirmOrResponse>;
    using PacketEventData = MACEventData<Packet>;
    using DataEventData = MACEventData<DLDataRequestParameter>;
    
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
        bool ConnectRequest(const DLConnectRequestOrIndication& Parameters);
        //
        // MA-CONNECT Service
        //
        bool DataRequest(const DLDataRequestParameter& Parameters);

        HDLCRunResult Process();
  
    private:
        //
        // State Machine
        //
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
        bool UI_Handler(const Packet& RXPacket);
        bool UA_Handler(const Packet& RXPacket);
       
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
        bool ConnectResponse(const DLConnectConfirmOrResponse& Parameters);
        //
        // MA-CONNECT Service
        //
        bool DataRequest(const DLDataRequestParameter& Parameters);

        HDLCRunResult Process();
        
    private:
        //
        // State Machine
        //
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
        bool SNRM_Handler(const Packet& Packet);
        bool I_Handler(const Packet& RXPacket);
        
    };


} /* namespace EPRI */

