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
        
        enum AuthenticationValueChoice : int8_t
        {
            charstring = 0,
            bitstring = 1,
            external = 2,
            other = 3
        };
        
    };

}