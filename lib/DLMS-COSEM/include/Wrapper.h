#pragma once

#include <functional>
#include "Callback.h"
#include "Transport.h"
#include "APDU/APDUFactory.h"

namespace EPRI
{
    class Wrapper : public Callback<bool, uint16_t>, public Transport
    {
    public:
        typedef std::pair<uint16_t, uint16_t> WrapperPorts;
            
        Wrapper() = delete;
        Wrapper(const WrapperPorts& Ports);
        virtual ~Wrapper();
    	
        virtual bool Process() = 0;

        WrapperPorts GetPorts() const;
        //
        // Transport
        //
        virtual bool DataRequest(const DataRequestParameter& Parameters);
        virtual void RegisterDataIndication(CallbackFunction Callback);
        
    protected:
        virtual bool Send(const DLMSVector& Data) = 0;
        virtual bool Receive(DLMSVector * pData) = 0;
        virtual bool ProcessReception(DLMSVector * pData);        
        
        APDUFactory     m_APDUFactory;
	
    private:
        WrapperPorts    m_Ports = {};
        
    };
	
} /* namespace EPRI */
