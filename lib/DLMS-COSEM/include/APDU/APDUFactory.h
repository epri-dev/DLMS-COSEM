#pragma once

#include <memory>

#include "DLMSVector.h"
#include "APDU.h"
#include "COSEMAddress.h"

namespace EPRI
{
    class DLMSVector;
    class IAPDU;
    
    class APDUFactory
    {
    public:
        APDUFactory();
        virtual ~APDUFactory();
        
        virtual IAPDUPtr Parse(COSEMAddressType SourceAddress,
            COSEMAddressType DestinationAddress, DLMSVector * pData);
        
    };

}