#include "serialwrapper/SerialWrapper.h"

namespace EPRI
{

    SerialWrapper::SerialWrapper(ISerial * pSerial) :
        m_pSerial(pSerial)
    {
    }
    
    SerialWrapper::~SerialWrapper()
    {
    }
    	
    bool SerialWrapper::Process()
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
        if ((RetVal = m_pSerial->Write(Data.GetData(), Data.Size())) != SUCCESSFUL) 
        {
            return false;
        }
        return true;
    }
    
    bool SerialWrapper::Receive(DLMSVector * pData)
    {
        uint8_t		   Byte;
        uint32_t       CharacterTimeout = 400;
        size_t         BytesReceived = 0;
        //
        // Check to see if there is a byte available.  If so, then we can just stream 
        // until we hit a character timeout.
        //
        ERROR_TYPE     RetVal = m_pSerial->Read(&Byte, sizeof(Byte), 0);
        while (SUCCESSFUL == RetVal)
        {
            pData->Append<uint8_t>(Byte);
            BytesReceived++;
            RetVal = m_pSerial->Read(&Byte, sizeof(Byte), CharacterTimeout);
        }
        return BytesReceived;
    }
	
} /* namespace EPRI */
