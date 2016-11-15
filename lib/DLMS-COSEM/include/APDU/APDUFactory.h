#pragma once

#include <memory>

#include "DLMSVector.h"
#include "APDU.h"

namespace EPRI
{
    class DLMSVector;
    class IAPDU;
    
    class APDUFactory
    {
    public:
        APDUFactory();
        virtual ~APDUFactory();
        
        virtual IAPDUPtr Parse(DLMSVector * pData);
        
    };

}