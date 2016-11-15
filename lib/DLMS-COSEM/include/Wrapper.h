#pragma once

#include <functional>
#include "Callback.h"
#include "Transport.h"

namespace EPRI
{
    class Wrapper : public Transport
    {
    public:
        typedef std::pair<uint16_t, uint16_t> WrapperPorts;
        const uint16_t                        CURRENT_VERSION = 0x0001;
            
        Wrapper() = delete;
        Wrapper(const WrapperPorts& Ports);
        virtual ~Wrapper();
    	
        virtual bool Process() = 0;

        WrapperPorts GetPorts() const;
        //
        // Transport
        //
        virtual bool DataRequest(const DataRequestParameter& Parameters);
        
    protected:
        virtual bool Send(const DLMSVector& Data) = 0;
        virtual bool Receive(DLMSVector * pData) = 0;
        virtual bool ProcessReception(DLMSVector * pData);        
	
    private:
        WrapperPorts    m_Ports = {};
        
    };
	
} /* namespace EPRI */
