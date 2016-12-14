#include "serialwrapper/SerialWrapper.h"

namespace EPRI
{

    SerialWrapper::SerialWrapper(ISerialSocket * pSerial) :
        m_pSerial(pSerial)
    {
    }
    
    SerialWrapper::~SerialWrapper()
    {
    }
    	
    Transport::ProcessResultType SerialWrapper::Process()
    {
        bool RetVal = true;
        if (!m_bConnectionFired)
        {
            FireTransportEvent(Transport::TRANSPORT_CONNECTED);
            m_bConnectionFired = true;
        }
        DLMSVector RxData;
        if (Receive(&RxData))
        {
            ProcessReception(&RxData);
        }
        return RetVal;
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
        uint32_t       CharacterTimeout = 400;
        size_t         BytesReceived = 0;
        //
        // Check to see if there is a byte available.  If so, then we can just stream 
        // until we hit a character timeout.
        //
        ERROR_TYPE     RetVal = m_pSerial->Read(pData, sizeof(uint8_t), 0);
        while (SUCCESSFUL == RetVal)
        {
            ++BytesReceived;
            RetVal = m_pSerial->Read(pData, sizeof(uint8_t), CharacterTimeout);
        }
        return BytesReceived;
    }
	
} /* namespace EPRI */
