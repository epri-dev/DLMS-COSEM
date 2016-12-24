#include "COSEMSecurity.h"

namespace EPRI
{
    const ASNObjectIdentifier COSEMSecurityOptions::ContextLNRNoCipher({ 2, 16, 756, 5, 8, 1, 1 });
    const ASNObjectIdentifier COSEMSecurityOptions::ContextSNRNoCipher({ 2, 16, 756, 5, 8, 1, 2 });
    const ASNObjectIdentifier COSEMSecurityOptions::ContextLNRCipher({ 2, 16, 756, 5, 8, 1, 3 });
    const ASNObjectIdentifier COSEMSecurityOptions::ContextSNRCipher({ 2, 16, 756, 5, 8, 1, 4 });
    const ASNObjectIdentifier COSEMSecurityOptions::MechanismNameLowLevelSecurity({ 2, 16, 756, 5, 8, 2, 1 }, 
                                                                                  ASN::IMPLICIT);
    const ASNObjectIdentifier COSEMSecurityOptions::MechanismNameHighLevelSecurity({ 2, 16, 756, 5, 8, 2, 5 }, 
                                                                                   ASN::IMPLICIT);
    
    COSEMSecurityOptions::COSEMSecurityOptions()
    {
    }
    
    COSEMSecurityOptions::~COSEMSecurityOptions()
    {
    }

    COSEMSecurityOptions::SecurityLevel COSEMSecurityOptions::Level() const
    {
        if (MechanismName == MechanismNameLowLevelSecurity)
        {
            return SecurityLevel::SECURITY_LOW_LEVEL;
        }
        else if (MechanismName == MechanismNameHighLevelSecurity)
        {
            return SecurityLevel::SECURITY_HIGH_LEVEL;
        }
        return SecurityLevel::SECURITY_NONE;
    }
    
    bool COSEMSecurityOptions::LogicalNameReferencing() const
    {
        return (ApplicationContextName == ContextLNRNoCipher ||
                ApplicationContextName == ContextLNRCipher);
    }
    
    bool COSEMSecurityOptions::Authentication() const
    {
        return IsInitialized(AuthenticationValue);
    }
    
    APDUConstants::AuthenticationValueChoice COSEMSecurityOptions::AuthenticationType() const
    {
        enum AuthenticationValueChoice : int8_t
        {
            charstring = 0,
            bitstring  = 1,
            external   = 2,
            other      = 3
        };
        if (IsSequence(AuthenticationValue))
        {
            return APDUConstants::AuthenticationValueChoice::other;
        }
        switch (VariantType(DLMSValueGetVariant(AuthenticationValue)))
        {
        case VAR_STRING:
            return APDUConstants::AuthenticationValueChoice::charstring;
        case VAR_BITSET:
            return APDUConstants::AuthenticationValueChoice::bitstring;
        case VAR_VECTOR:
            return APDUConstants::AuthenticationValueChoice::external;
        default:
            return APDUConstants::AuthenticationValueChoice::other;
        }
    }        
        
}