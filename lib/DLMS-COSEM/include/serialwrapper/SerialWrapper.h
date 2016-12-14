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
        
        void Serial_Connect(ERROR_TYPE Error);
        void Serial_Receive(ERROR_TYPE Error, size_t BytesReceived);
        void Serial_Close(ERROR_TYPE Error);
        
        bool ArmAsyncRead(size_t MinimumSize = Wrapper::HEADER_SIZE);

    private:
        enum ReadState
        {
            HEADER_WAIT,
            BODY_WAIT
        }               m_ReadState = HEADER_WAIT;
        ISerialSocket * m_pSerial = nullptr;
        DLMSVector      m_RXVector;
    };
	
} /* namespace EPRI */
