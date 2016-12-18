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
    class ISerialSocket;
    class DLDataRequestParameter;

    class HDLCMAC : public Callback<bool, uint16_t>, public StateMachine
	{
    	using PacketCallback = Callback<bool, HDLCControl::Control, Packet>;
    	
	public:
    	HDLCMAC() = delete;
    	//
    	// DL-INITIALIZE
    	//
    	HDLCMAC(const HDLCAddress& MyAddress, 
        	ISerialSocket * pSerial, 
        	const HDLCOptions& Options,
        	uint8_t MaxPreallocatedPacketBuffers);
    	virtual ~HDLCMAC();
    	
    	HDLCAddress MyAddress() const;
    	const HDLCStatistics& Statistics() const;
    	void ClearStatistics();
    	
    	virtual HDLCAddress ConnectedAddress() const;
    	virtual bool IsConnected() const;
    	//
    	// DL-SET_VALUE
    	//
    	virtual void SetOptions(const HDLCOptions& Options);
    	//
    	// DL-GET_VALUE
    	//
    	virtual HDLCOptions GetOptions() const;
        //
        // MA-DATA Service
        //
    	virtual bool DataRequest(const DLDataRequestParameter& Parameters);
    	
	protected:
        //
        // State Machine/DL-LM_EVENT
        //
    	enum States : uint8_t
    	{
        	ST_DISCONNECTED    = 0,
        	ST_IEC_CONNECT,
        	ST_CONNECTING_WAIT,
        	ST_CONNECTED,
        	ST_MAX_STATES
    	};

    	virtual void ST_Disconnected_Handler(EventData * pData) = 0;
    	virtual void ST_IEC_Connect_Handler(EventData * pData) = 0;
    	virtual void ST_Connecting_Wait_Handler(EventData * pData) = 0;
    	virtual void ST_Connected_Handler(EventData * pData);
    	//
    	// Packet Handlers
    	//
    	virtual bool I_Handler(const Packet& RXPacket);
    	
    	using PacketPtr = std::unique_ptr<Packet>;

    	virtual void Process() = 0;
    	
    	virtual HDLCErrorCode ProcessSerialReception(ERROR_TYPE Error, size_t BytesReceived);
    	Packet * GetWorkingRXPacket();
    	void ReleaseWorkingRXPacket();
    	void EnqueueWorkingRXPacket();            

    	virtual void ProcessSerialTransmission();
    	Packet * GetWorkingTXPacket();
    	void ReleaseWorkingTXPacket();
    	void EnqueueWorkingTXPacket();            
    	Packet * GetOutgoingPacket();
    	void ReleaseOutgoingPacket();
    	
    	virtual HDLCErrorCode ProcessPacketReception();
    	Packet * GetIncomingPacket();
    	void ReleaseIncomingPacket();
    	
    	void Serial_Connect(ERROR_TYPE Error);
        void Serial_Receive(ERROR_TYPE Error, size_t BytesReceived);
    	void Serial_Close(ERROR_TYPE Error);

    	bool ArmAsyncRead(uint32_t TimeOutInMs = 0, size_t MinimumSize = sizeof(uint8_t));
      	
        HDLCAddress							m_MyAddress;
    	HDLCAddress                         m_ConnectedAddress;
    	ISerialSocket *                     m_pSerial;
    	HDLCOptions                         m_CurrentOptions;
		std::shared_ptr<EPRI::ISimpleTimer>	m_pTimer;
		HDLCStatistics					    m_Statistics;
    	std::queue<PacketPtr>               m_Packets;
    	std::queue<PacketPtr>               m_RXPackets;
    	std::queue<PacketPtr>               m_TXPackets;
    	PacketCallback                      m_PacketCallback;
    	DLMSVector                          m_RXVector;
    	
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
    // IDENTIFY
    //
    struct DLIdentifyRequestParameter : public HDLCCallbackParameter
    {
        static const uint16_t ID = 0x100A;
        DLIdentifyRequestParameter(const HDLCAddress& DA)
            : HDLCCallbackParameter(DA)
        {
        }
    };
    struct DLIdentifyResponseParameter : public HDLCCallbackParameter
    {
        static const uint16_t ID = 0x100B;
        DLIdentifyResponseParameter(const HDLCAddress& DA, const uint8_t * pData)
            : HDLCCallbackParameter(DA)
        {
            SuccessCode = pData[0];
            ProtocolID = pData[1];
            ProtocolVersion = pData[2];
            ProtocolRevision = pData[3];
        }
        DLIdentifyResponseParameter(const HDLCAddress& DA)
            : HDLCCallbackParameter(DA)
        {
            SuccessCode = Packet::IDENTIFY_RESPONSE[0];
            ProtocolID = Packet::IDENTIFY_RESPONSE[1];
            ProtocolVersion = Packet::IDENTIFY_RESPONSE[2];
            ProtocolRevision = Packet::IDENTIFY_RESPONSE[3];
        }
        uint8_t SuccessCode;
        uint8_t ProtocolID;
        uint8_t ProtocolVersion;
        uint8_t ProtocolRevision;
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
    //
    // DISCONNECT
    //
    struct DLDisconnectConfirmOrResponse : public HDLCCallbackParameter
    {
        static const uint16_t ID = 0x100A;
        DLDisconnectConfirmOrResponse(const HDLCAddress& DA)
            : HDLCCallbackParameter(DA)
        {
        }
        // Result
        // User_Information
    };
    
    struct DLDisconnectRequestOrIndication : public HDLCCallbackParameter
    {
        static const uint16_t ID = 0x100B;
        DLDisconnectRequestOrIndication(const HDLCAddress& DA)
            : HDLCCallbackParameter(DA)
        {
        }
        DLDisconnectRequestOrIndication()
            : HDLCCallbackParameter(HDLCAddress())
        {
        }

        // User_Information
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
    
    using IdentifyEventData = MACEventData<DLIdentifyRequestParameter>;
    using IdentifyResponseData = MACEventData<DLIdentifyResponseParameter>;
    using ConnectEventData = MACEventData<DLConnectRequestOrIndication>;
    using ConnectResponseData = MACEventData<DLConnectConfirmOrResponse>;
    using PacketEventData = MACEventData<Packet>;
    using DataEventData = MACEventData<DLDataRequestParameter>;
    using DisconnectEventData = MACEventData<DLDisconnectRequestOrIndication>;
    using DisconnectResponseData = MACEventData<DLDisconnectConfirmOrResponse>;
    
    class HDLCClient : public HDLCMAC
    {
    public:
        HDLCClient() = delete;
    	//
    	// DL-INITIALIZE, DL-SETVALUE(HDLCOptions)
    	//
        HDLCClient(const HDLCAddress& MyAddress, 
            ISerialSocket * pSerial, 
            const HDLCOptions& Opt,
            uint8_t MaxPreallocatedPacketBuffers);
        virtual ~HDLCClient();
    	//
    	// IDENTIFY Service
    	//
        virtual bool IdentifyRequest(const DLIdentifyRequestParameter& Parameters);
        //
        // MA-CONNECT Service
        //
        virtual bool ConnectRequest(const DLConnectRequestOrIndication& Parameters);
        //
        // MA-DISCONNECT Service
        //
        virtual bool DisconnectRequest(const DLDisconnectRequestOrIndication& Parameters);
        
    protected:
        //
        // HDLCMAC
        //
        void Process();
  
    private:
        virtual void ST_Disconnected_Handler(EventData * pData) final;
        virtual void ST_IEC_Connect_Handler(EventData * pData) final;
        virtual void ST_Connecting_Wait_Handler(EventData * pData) final;
        virtual void ST_Connected_Handler(EventData * pData) final;
        //
        // Packet Handlers
        //
        bool UI_Handler(const Packet& RXPacket);
        bool UA_Handler(const Packet& RXPacket);
        bool DM_Handler(const Packet& RXPacket);
        bool IDENTR_Handler(const Packet& RXPacket);
      
    };
    
    class HDLCServer : public HDLCMAC
    {
    public:
        HDLCServer() = delete;
    	//
    	// DL-INITIALIZE, DL-SETVALUE(HDLCOptions)
    	//
        HDLCServer(const HDLCAddress& MyAddress, 
            ISerialSocket * pSerial, 
            const HDLCOptions& Opt,
            uint8_t MaxPreallocatedPacketBuffers);
        virtual ~HDLCServer();
        //
        // IDENTIFY Service
        //
        bool IdentifyResponse(const DLIdentifyResponseParameter& Parameters);
        //
        // MA-CONNECT Service
        //
        bool ConnectResponse(const DLConnectConfirmOrResponse& Parameters);
        //
        // MA-DISCONNECT Service
        //
        bool DisconnectResponse(const DLDisconnectConfirmOrResponse& Parameters);
        
    protected:
        //
        // HDLCMAC
        //
        void Process();
        
    private:
        virtual void ST_Disconnected_Handler(EventData * pData) final;
        virtual void ST_IEC_Connect_Handler(EventData * pData) final;
        virtual void ST_Connecting_Wait_Handler(EventData * pData) final;
        virtual void ST_Connected_Handler(EventData * pData) final;
        //
        // Packet Handlers
        //
        bool SNRM_Handler(const Packet& Packet);
        bool IDENT_Handler(const Packet& RXPacket);
        bool DISC_Handler(const Packet& RXPacket);
       
    };


} /* namespace EPRI */

