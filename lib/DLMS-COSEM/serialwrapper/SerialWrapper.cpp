#include "serialwrapper/SerialWrapper.h"

namespace EPRI
{

    SerialWrapper::SerialWrapper(ISerialSocket * pSerial) :
        m_pSerial(pSerial)
    {
        m_pSerial->RegisterConnectHandler(
            std::bind(&SerialWrapper::Serial_Connect, this, std::placeholders::_1));
        m_pSerial->RegisterReadHandler(
            std::bind(&SerialWrapper::Serial_Receive, this, std::placeholders::_1, std::placeholders::_2));
        m_pSerial->RegisterCloseHandler(
            std::bind(&SerialWrapper::Serial_Close, this, std::placeholders::_1));
    }
    
    SerialWrapper::~SerialWrapper()
    {
    }
    	
    Transport::ProcessResultType SerialWrapper::Process()
    {
        return true;
    }

    bool SerialWrapper::Send(const DLMSVector& Data)
    {
        ERROR_TYPE     RetVal = SUCCESSFUL;
        if ((RetVal = m_pSerial->Write(Data)) != SUCCESSFUL) 
        {
            return false;
        }
        return true;
    }
    
    bool SerialWrapper::Receive(DLMSVector * pData)
    {
        //
        // Not implemented
        //
        return false;
    }
    
    void SerialWrapper::Serial_Receive(ERROR_TYPE Error, size_t BytesReceived)
    {
        if (SUCCESSFUL == Error || BytesReceived)
        {
            m_pSerial->AppendAsyncReadResult(&m_RXVector, BytesReceived);
            switch (m_ReadState)
            {
            case HEADER_WAIT:
                if (m_RXVector.Size() == Wrapper::HEADER_SIZE)
                {
                    ArmAsyncRead(ParseMessageLength(m_RXVector));
                    m_ReadState = BODY_WAIT;
                }
                break;
            case BODY_WAIT:
                if (m_RXVector.Size() == Wrapper::HEADER_SIZE + ParseMessageLength(m_RXVector))
                {
                    ProcessReception(&m_RXVector);
                    //
                    // Rearm Reception
                    //
                    m_RXVector.Clear();
                    ArmAsyncRead();
                    m_ReadState = HEADER_WAIT;
                }
                break;
            }
        }
    }
    
    void SerialWrapper::Serial_Connect(ERROR_TYPE Error)
    {
        if (!Error)
        {
            FireTransportEvent(Transport::TRANSPORT_CONNECTED);
            //
            // Arm read to catch a header at least
            //
            ArmAsyncRead();
        }
    }
    
    void SerialWrapper::Serial_Close(ERROR_TYPE Error)
    {
        FireTransportEvent(Transport::TRANSPORT_DISCONNECTED);
    }
    
    bool SerialWrapper::ArmAsyncRead(size_t MinimumSize /* = Wrapper::HEADER_SIZE*/)
    {
        return SUCCESSFUL == m_pSerial->Read(nullptr, MinimumSize);
    }    
	
} /* namespace EPRI */
