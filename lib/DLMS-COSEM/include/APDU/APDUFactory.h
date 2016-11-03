#pragma once

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
        
        virtual IAPDU * Parse(DLMSVector * pData);
        
    };

}