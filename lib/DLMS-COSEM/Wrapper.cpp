#include "Wrapper.h"
#include "APDU/APDU.h"

namespace EPRI
{

    Wrapper::Wrapper(const WrapperPorts& Ports)
        : m_Ports(Ports)
    {
    }
    
    Wrapper::~Wrapper()
    {
    }
    	
    Wrapper::WrapperPorts Wrapper::GetPorts() const
    {
        return m_Ports;
    }

    bool Wrapper::DataRequest(const DataRequestParameter& Parameters)
    {
        //
        // Put the wrapper header on the request and send it.
        //
        DLMSVector  Request;
        Request.Append<uint16_t>(CURRENT_VERSION);
        Request.Append<uint16_t>(m_Ports.first);
        Request.Append<uint16_t>(m_Ports.second);
        Request.Append<uint16_t>(Parameters.Data.Size());
        Request.Append(Parameters.Data);
        return (Send(Request));
    }

    bool Wrapper::ProcessReception(DLMSVector * pData)
    {
        bool RetVal = false;
        try
        {
            //
            // Remove the wrapper header and validate.
            //
            if (pData->Get<uint16_t>() == CURRENT_VERSION &&
                pData->Get<uint16_t>() == m_Ports.first &&
                pData->Get<uint16_t>() == m_Ports.second)
            {
                size_t Length = pData->Get<uint16_t>();
                if (Length)
                {
                    pData->RemoveReadBytes();
                    RetVal = Transport::ProcessReception(pData);
                }
            }
        }
        catch (const std::exception&)
        {
            RetVal = false;
        }
        return RetVal;
    }
	
} /* namespace EPRI */
