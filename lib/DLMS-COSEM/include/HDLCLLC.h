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
    	HDLCLLC(HDLCMAC * pMAC);
    	virtual ~HDLCLLC();
    	
    	virtual HDLCRunResult Process();

    	HDLCAddress MyAddress() const;
    	const HDLCStatistics& Statistics() const;
    	void ClearStatistics();
    	//
        // DL-DATA Service
        //
    	virtual HDLCRunResult DataRequest(const DLDataRequestParameter& Parameters) = 0;
    	virtual void RegisterDataIndication(CallbackFunction Callback) = 0;
	
	protected:
    	DLDataRequestParameter& AddLLCHeader(DLDataRequestParameter * pParameters);
    	
	private:
      	HDLCMAC *				 m_pMAC;
	};
    
    class HDLCClientLLC : public HDLCLLC
    {
    public:
        HDLCClientLLC(const HDLCAddress& MyAddress, 
            ISerial * pSerial, 
            const HDLCOptions& Options,
            uint8_t MaxPreallocatedPacketBuffers = 10);
        virtual ~HDLCClientLLC();
        
        bool IsConnected() const;
        HDLCAddress ConnectedAddress() const;
        //
        // DL-CONNECT Service
        //
        HDLCRunResult ConnectRequest(const DLConnectRequestOrIndication& Parameters);
        void RegisterConnectConfirm(CallbackFunction Callback);
        //
        // DL-DATA Service
        //
        HDLCRunResult DataRequest(const DLDataRequestParameter& Parameters);
        void RegisterDataIndication(CallbackFunction Callback);
        //
        // Transport
        //
        bool DataRequest(const DataRequestParameter& Parameters);

    protected:
        bool MACConnectConfirm(const BaseCallbackParameter& Paramters);
        bool MACDataIndication(const BaseCallbackParameter& Parameters);
        
    private:
        HDLCClient              m_MAC;
        HDLCAddress             m_ConnectedAddress;
        
    };
    
    class HDLCServerLLC : public HDLCLLC
    {
    public:
        HDLCServerLLC(const HDLCAddress& MyAddress, 
            ISerial * pSerial, 
            const HDLCOptions& Options,
            uint8_t MaxPreallocatedPacketBuffers = 10);
        virtual ~HDLCServerLLC();
        //
        // DL-CONNECT Service
        //
        void RegisterConnectIndication(CallbackFunction Callback);
        HDLCRunResult ConnectResponse(const DLConnectConfirmOrResponse& Parameters);
        //
        // DL-DATA Service
        //
        HDLCRunResult DataRequest(const DLDataRequestParameter& Parameters);
        void RegisterDataIndication(CallbackFunction Callback);
        //
        // Transport
        //
        bool DataRequest(const DataRequestParameter& Parameters);
        
    protected:
        bool MACConnectIndication(const BaseCallbackParameter& Parameters);
        bool MACDataIndication(const BaseCallbackParameter& Parameters);
        
    private:
        HDLCServer              m_MAC;
        
    };

	
} /* namespace EPRI */
