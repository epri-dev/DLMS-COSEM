#include "APDU/AARQ.h"

namespace EPRI
{
    const ASNType AARQ::protocol_version_default = ASNBitString(1, 0x00);
    
    ASN_BEGIN_SCHEMA(AARQ::protocol_version_Schema)
        ASN_BIT_STRING_TYPE(ASN::IMPLICIT, 1)
    ASN_END_SCHEMA
    ASN_BEGIN_SCHEMA(AARQ::sender_acse_requirements_Schema)
        ASN_BIT_STRING_TYPE(ASN::NO_OPTIONS, 1)
    ASN_END_SCHEMA
    ASN_BEGIN_SCHEMA(AARQ::calling_authentication_value_Schema)
        ASN_BEGIN_CHOICE
            ASN_GraphicString_TYPE(ASN::IMPLICIT)
        ASN_END_CHOICE
    ASN_END_SCHEMA

    AARQ::AARQ()
    {
        ASN_BEGIN_COMPONENTS
            ASN_COMPONENT(protocol_version)
            ASN_COMPONENT(application_context_name)
            ASN_COMPONENT(called_AP_title)
            ASN_COMPONENT(called_AE_qualifier)
            ASN_COMPONENT(called_AP_invocation_id)
            ASN_COMPONENT(called_AE_invocation_id)
            ASN_COMPONENT(calling_AP_title)
            ASN_COMPONENT(calling_AE_qualifier)
            ASN_COMPONENT(calling_AP_invocation_id)
            ASN_COMPONENT(calling_AE_invocation_id)
            ASN_COMPONENT(sender_acse_requirements)
            ASN_COMPONENT(mechanism_name)
            ASN_COMPONENT(calling_authentication_value)
            ASN_COMPONENT(implementation_information)
            ASN_COMPONENT(user_information)
        ASN_END_COMPONENTS
    }
    
    AARQ::~AARQ()
    {
    }

}