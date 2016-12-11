#include "APDU/APDUFactory.h"
#include "APDU/APDUDefs.h"

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
        case Set_Request_Base::Tag:
            //
            // TODO - Appropriate SET types...
            //
            pRetVal.reset(new Set_Request_Normal());
            break;
        case Set_Response_Base::Tag:
            pRetVal.reset(new Set_Response_Normal());
            break;
        case Action_Request_Base::Tag:
            //
            // TODO - Appropriate ACTION types...
            //
            pRetVal.reset(new Action_Request_Normal());
            break;
        case Action_Response_Base::Tag:
            pRetVal.reset(new Action_Response_Normal());
            break;
        case RLRQ::Tag:
            pRetVal.reset(new RLRQ());
            break;
        case RLRE::Tag:
            pRetVal.reset(new RLRE());
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