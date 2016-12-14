#pragma once

#include "ISerial.h"
#include "Wrapper.h"

namespace EPRI
{
    class SerialWrapper : public Wrapper
    {
    public:
        SerialWrapper() = delete;
        SerialWrapper(ISerialSocket * pSerial);
        virtual ~SerialWrapper();
    	//
        // Transport
        //
        virtual ProcessResultType Process();
	
    protected:
        //
        // Transport
        //
        virtual bool OnConnect(COSEMAddressType Address)
        {
            return true;
        }
        
        bool Send(const DLMSVector& Data);
        bool Receive(DLMSVector * pData);
 
    private:
        ISerialSocket * m_pSerial = nullptr;
        bool            m_bConnectionFired = false;
        
    };
	
} /* namespace EPRI */
