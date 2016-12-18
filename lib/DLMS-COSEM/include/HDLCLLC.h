#pragma once

#include <functional>
#include "ISerial.h"
#include "hdlc/HDLCMAC.h"
#include "hdlc/HDLCOptions.h"
#include "Callback.h"
#include "Transport.h"

namespace EPRI
{
    class HDLCLLC : public Transport
	{
	public:
    	HDLCLLC() = delete;
    	//
    	// DL-INITIALIZE
    	//
    	HDLCLLC(HDLCMAC * pMAC);
    	virtual ~HDLCLLC();
    	
    	virtual ProcessResultType Process();

    	HDLCAddress MyAddress() const;
    	const HDLCStatistics& Statistics() const;
    	void ClearStatistics();
    	
    	virtual HDLCAddress ConnectedAddress() const;
    	virtual bool IsConnected() const;
    	//
        // DL-DATA Service
        //
    	virtual bool DataRequest(const DLDataRequestParameter& Parameters);
        //
        // Transport
        //
    	bool DataRequest(const DataRequestParameter& Parameters);
	
	protected:
        //
        // DL-CONNECT Service
        //
    	virtual bool MACConnectConfirmOrIndication(const BaseCallbackParameter& Paramters);
        //
        // DL-DATA Service
        //
    	virtual bool MACDataIndication(const BaseCallbackParameter& Parameters);
        //
        // DL-DISCONNECT Service
        //
    	virtual bool MACDisconnectConfirmOrIndication(const BaseCallbackParameter& Paramters);
        //
    	DLDataRequestParameter& AddLLCHeader(DLDataRequestParameter * pParameters);
    	
	private:
      	HDLCMAC *				 m_pMAC;
	};
    
    class HDLCClientLLC : public HDLCLLC
    {
    public:
    	//
    	// DL-INITIALIZE
    	//
        HDLCClientLLC(const HDLCAddress& MyAddress, 
            ISerialSocket * pSerial, 
            const HDLCOptions& Options,
            uint8_t MaxPreallocatedPacketBuffers = 10);
        virtual ~HDLCClientLLC();
    	//
    	// IDENTIFY Service
    	//
        virtual bool IdentifyRequest(const DLIdentifyRequestParameter& Parameters);
        void RegisterIdentifyConfirm(CallbackFunction Callback);
        //
        // DL-CONNECT Service
        //
        bool ConnectRequest(const DLConnectRequestOrIndication& Parameters);
        void RegisterConnectConfirm(CallbackFunction Callback);
        //
        // DL-DATA Service
        //
        void RegisterDataIndication(CallbackFunction Callback);
        //
        // DL-DISCONNECT Service
        //
        bool DisconnectRequest(const DLDisconnectRequestOrIndication& Parameters);
        void RegisterDisconnectConfirm(CallbackFunction Callback);

    protected:
        //
        // IDENTIFY Service
        //
        virtual bool MACIdentifyConfirm(const BaseCallbackParameter& Parameters);
        //
        // DL-CONNECT Service
        //
        virtual bool MACConnectConfirm(const BaseCallbackParameter& Paramters);
    	//
        // DL-DATA Service
        //
        virtual bool MACDataIndication(const BaseCallbackParameter& Parameters) final;
        //
        // DL-DISCONNECT Service
        //
        virtual bool MACDisconnectConfirm(const BaseCallbackParameter& Paramters);
        
    private:
        HDLCClient              m_MAC;
        
    };
    
    class HDLCServerLLC : public HDLCLLC
    {
    public:
    	//
    	// DL-INITIALIZE
    	//
        HDLCServerLLC(const HDLCAddress& MyAddress, 
            ISerialSocket * pSerial, 
            const HDLCOptions& Options,
            uint8_t MaxPreallocatedPacketBuffers = 10);
        virtual ~HDLCServerLLC();
        //
        // DL-CONNECT Service
        //
        bool ConnectResponse(const DLConnectConfirmOrResponse& Parameters);
        //
        // DL-DISCONNECT Service
        //
        bool DisconnectResponse(const DLDisconnectConfirmOrResponse& Parameters);
        
    protected:
        //
        // IDENTIFY Service (Handled Completely in the Server)
        //
        virtual bool IdentifyResponse(const DLIdentifyResponseParameter& Parameters);
        virtual bool MACIdentifyIndication(const BaseCallbackParameter& Parameters);
        //
        // DL-CONNECT Service
        //
        virtual bool OnConnectIndication(COSEMAddressType Address);
        virtual bool MACConnectIndication(const BaseCallbackParameter& Parameters);
        //
        // DL-DISCONNECT Service
        //
        virtual bool OnDisconnectIndication(COSEMAddressType Address);
        virtual bool MACDisconnectIndication(const BaseCallbackParameter& Parameters);
        
    private:
        HDLCServer              m_MAC;
    };

	
} /* namespace EPRI */
