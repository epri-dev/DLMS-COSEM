#pragma once

#include "ISerial.h"
#include "Wrapper.h"

namespace EPRI
{
    class SerialWrapper : public Wrapper
    {
    public:
        SerialWrapper() = delete;
        SerialWrapper(ISerial * pSerial, const WrapperPorts& Ports);
        virtual ~SerialWrapper();
    	
        virtual bool Process();
	
    protected:
        bool Send(const DLMSVector& Data);
        bool Receive(DLMSVector * pData);
 
    private:
        ISerial *	    m_pSerial = nullptr;
        bool            m_bConnectionFired = false;
        
    };
	
} /* namespace EPRI */
