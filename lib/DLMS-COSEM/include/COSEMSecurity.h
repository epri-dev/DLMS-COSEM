#pragma once

#include "APDU/ASNType.h"
#include "APDU/APDUConstants.h"
#include "DLMSVector.h"

namespace EPRI
{
    struct COSEMSecurityOptions
    {
        using AuthenticationValueType = DLMSValue;
        
        static const ASNObjectIdentifier ContextLNRNoCipher;
        static const ASNObjectIdentifier ContextSNRNoCipher;
        static const ASNObjectIdentifier ContextLNRCipher;
        static const ASNObjectIdentifier ContextSNRCipher;
        static const ASNObjectIdentifier MechanismNameLowLevelSecurity;
        static const ASNObjectIdentifier MechanismNameHighLevelSecurity;
        
        enum SecurityLevel : uint8_t
        {
            SECURITY_NONE       = 0,
            SECURITY_LOW_LEVEL  = 1,
            SECURITY_HIGH_LEVEL = 2
        };
        
        COSEMSecurityOptions();
        virtual ~COSEMSecurityOptions();

        SecurityLevel Level() const;
        bool LogicalNameReferencing() const;
        bool Authentication() const;
        APDUConstants::AuthenticationValueChoice AuthenticationType() const;        
        
        ASNObjectIdentifier     ApplicationContextName;
        ASNObjectIdentifier     MechanismName;
        AuthenticationValueType AuthenticationValue;
    };
    
}
