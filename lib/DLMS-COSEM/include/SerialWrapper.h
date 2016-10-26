#pragma once

#include <functional>
#include "ISerial.h"
#include "Callback.h"
#include "Transport.h"

namespace EPRI
{
    class SerialWrapper : public Callback<bool, uint16_t>, public Transport
    {
    public:
        typedef std::pair<uint16_t, uint16_t> WrapperPorts;
            
        SerialWrapper() = delete;
        SerialWrapper(ISerial * pSerial, const WrapperPorts& Ports);
        virtual ~SerialWrapper();
    	
        virtual bool Process();

        WrapperPorts GetPorts() const;
        //
        // Transport
        //
        virtual bool DataRequest(const DataRequestParameter& Parameters);
        virtual void RegisterDataIndication(CallbackFunction Callback);
	
    private:
        bool ProcessSerialReception();
        
        ISerial *	    m_pSerial = nullptr;
        WrapperPorts    m_Ports = { };
        bool            m_bConnectionFired = false;
        
    };
	
} /* namespace EPRI */
