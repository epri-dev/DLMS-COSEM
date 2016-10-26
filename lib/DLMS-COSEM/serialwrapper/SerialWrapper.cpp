#include "SerialWrapper.h"

namespace EPRI
{

    SerialWrapper::SerialWrapper(ISerial * pSerial, const WrapperPorts& Ports) :
        m_pSerial(pSerial),
        m_Ports(Ports)
    {
    }
    
    SerialWrapper::~SerialWrapper()
    {
    }
    	
    bool SerialWrapper::Process()
    {
        if (!m_bConnectionFired)
        {
            FireTransportEvent(Transport::TRANSPORT_CONNECTED);
            m_bConnectionFired = true;
        }
        ProcessSerialReception();
        return true;
    }

    SerialWrapper::WrapperPorts SerialWrapper::GetPorts() const
    {
        return m_Ports;
    }

    bool SerialWrapper::DataRequest(const DataRequestParameter& Parameters)
    {
        DataRequestParameter WrapperParam = Parameters;
        uint8_t              WRAPPER_HEADER[8];
        int                  Index = 0;
        WRAPPER_HEADER[Index++] = 0x00;
        WRAPPER_HEADER[Index++] = 0x01;
        WRAPPER_HEADER[Index++] = (0xFF & (m_Ports.first >> 8));
        WRAPPER_HEADER[Index++] = (0xFF & m_Ports.first);
        WRAPPER_HEADER[Index++] = (0xFF & (m_Ports.second >> 8));
        WRAPPER_HEADER[Index++] = (0xFF & m_Ports.second);
        WRAPPER_HEADER[Index++] = (0xFF & (Parameters.Data.size() >> 8));
        WRAPPER_HEADER[Index++] = (0xFF & Parameters.Data.size());
        WrapperParam.Data.insert(WrapperParam.Data.begin(), 
            WRAPPER_HEADER,
            WRAPPER_HEADER + sizeof(WRAPPER_HEADER));
        return (m_pSerial->Write(WrapperParam.Data.data(), WrapperParam.Data.size()) == SUCCESSFUL);
    }
    
    void SerialWrapper::RegisterDataIndication(CallbackFunction Callback)
    {
        RegisterCallback(DataRequestParameter::ID, Callback);
    }
    
    bool SerialWrapper::ProcessSerialReception()
    {
        uint8_t		   Byte;
        uint32_t       CharacterTimeout = 0;

        while (m_pSerial->Read(&Byte, sizeof(Byte), CharacterTimeout) == SUCCESSFUL)
        {
        }
    	
        return true;
    }

	
} /* namespace EPRI */
