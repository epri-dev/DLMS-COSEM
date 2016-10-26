#pragma once

#include "ASNType.h"

namespace EPRI
{
    struct APDUConstants
    {
        static const ASNType protocol_version_default;
        
        ASN_DEFINE_SCHEMA(protocol_version_Schema)
        ASN_DEFINE_SCHEMA(authentication_value_Schema)
        ASN_DEFINE_SCHEMA(acse_requirements_Schema)
            
    };

}