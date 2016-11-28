#include "Wrapper.h"
#include "APDU/APDU.h"

namespace EPRI
{

    Wrapper::Wrapper()
    {
    }
    
    Wrapper::~Wrapper()
    {
    }

    bool Wrapper::DataRequest(const DataRequestParameter& Parameters)
    {
        //
        // Put the wrapper header on the request and send it.
        //
        DLMSVector  Request;
        Request.Append<uint16_t>(CURRENT_VERSION);
        Request.Append<COSEMAddressType>(Parameters.SourceAddress);
        Request.Append<COSEMAddressType>(Parameters.DestinationAddress);
        Request.Append<uint16_t>(Parameters.Data.Size());
        Request.Append(Parameters.Data);
        return (Send(Request));
    }

    bool Wrapper::ProcessReception(DLMSVector * pData)
    {
        bool             RetVal = false;
        COSEMAddressType SourceAddress;
        COSEMAddressType DestinationAddress;
        try
        {
            //
            // Remove the wrapper header and validate.
            //
            if (pData->Get<uint16_t>() == CURRENT_VERSION)
            {
                SourceAddress = pData->Get<COSEMAddressType>();
                DestinationAddress = pData->Get<COSEMAddressType>();                
                size_t Length = pData->Get<uint16_t>();
                if (Length)
                {
                    pData->RemoveReadBytes();
                    RetVal = Transport::ProcessReception(SourceAddress, DestinationAddress, pData);
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
