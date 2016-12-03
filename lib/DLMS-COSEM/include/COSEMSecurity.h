#pragma once

#include "COSEM.h"

namespace EPRI
{
    struct COSEMSecurityOptions
    {
        COSEM::SecurityLevel Level;
        std::string          Password;  
    };
    
}
