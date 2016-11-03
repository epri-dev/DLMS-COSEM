#include "serialwrapper/SerialWrapper.h"

namespace EPRI
{

    SerialWrapper::SerialWrapper(ISerial * pSerial, const WrapperPorts& Ports) :
        Wrapper(Ports),
        m_pSerial(pSerial)
    {
    }
    
    SerialWrapper::~SerialWrapper()
    {
    }
    	
    bool SerialWrapper::Process()
    {
        bool RetVal = false;
        if (!m_bConnectionFired)
        {
            FireTransportEvent(Transport::TRANSPORT_CONNECTED);
            m_bConnectionFired = true;
        }
        DLMSVector RxData;
        if (Receive(&RxData))
        {
            RetVal = ProcessReception(&RxData);
        }
        return RetVal;
    }

    bool SerialWrapper::Receive(DLMSVector * pData)
    {
        uint8_t		   Byte;
        uint32_t       CharacterTimeout = 0;
        ERROR_TYPE     RetVal = SUCCESSFUL;

        while ((RetVal = m_pSerial->Read(&Byte, sizeof(Byte), CharacterTimeout)) == SUCCESSFUL)
        {
            pData->Append<uint8_t>(Byte);
        }
        return true;
    }
	
} /* namespace EPRI */
