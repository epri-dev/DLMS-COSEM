#include "APDU/APDUFactory.h"
#include "APDU/AARE.h"
#include "APDU/AARQ.h"
#include "APDU/GET-Request.h"
#include "APDU/GET-Response.h"

namespace EPRI
{
    APDUFactory::APDUFactory()
    {
    }
    
    APDUFactory::~APDUFactory()
    {
    }

    IAPDUPtr APDUFactory::Parse(COSEMAddressType SourceAddress,
        COSEMAddressType DestinationAddress,
        DLMSVector * pData)
    {
        IAPDUPtr pRetVal = nullptr;
        switch (pData->PeekByte())
        {
        case AARQ::Tag:
            pRetVal.reset(new AARQ());
            break;
        case AARE::Tag:
            pRetVal.reset(new AARE());
            break;
        case Get_Request_Base::Tag:
            //
            // TODO - Appropriate GET types...
            //
            pRetVal.reset(new Get_Request_Normal());
            break;
        case Get_Response_Base::Tag:
            pRetVal.reset(new Get_Response_Normal());
            break;
        default:
            break;
        }
        if (pRetVal && 
            !pRetVal->Parse(SourceAddress, DestinationAddress, pData))
        {
            pRetVal.release();
        }
        return pRetVal;
    }
    
}