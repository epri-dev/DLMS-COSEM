#include "APDU/APDUFactory.h"
#include "APDU/AARE.h"

namespace EPRI
{
    APDUFactory::APDUFactory()
    {
    }
    
    APDUFactory::~APDUFactory()
    {
    }

    IAPDUPtr APDUFactory::Parse(DLMSVector * pData)
    {
        IAPDUPtr pRetVal = nullptr;
        switch (pData->PeekByte())
        {
        case AARE::Tag:
            pRetVal.reset(new AARE());
            if (!pRetVal->Parse(pData))
            {
                pRetVal.release();
            }
            break;
        default:
            break;
        }
        return pRetVal;
    }
    
}