#include "APDU/AARE.h"

namespace EPRI
{
    ASN_BEGIN_SCHEMA(AARE::Association_Result_Schema)
        ASN_INTEGER_TYPE(ASN::NO_OPTIONS)
    ASN_END_SCHEMA
    ASN_BEGIN_SCHEMA(AARE::Associate_Source_Diagnostic_Schema)
        ASN_BEGIN_CHOICE
            ASN_INTEGER_TYPE(ASN::NO_OPTIONS)
            ASN_INTEGER_TYPE(ASN::NO_OPTIONS)        
        ASN_END_CHOICE
    ASN_END_SCHEMA

    AARE::AARE()
    {
        ASN_BEGIN_COMPONENTS
            ASN_COMPONENT(protocol_version)
            ASN_COMPONENT(application_context_name)
            ASN_COMPONENT(result)
            ASN_COMPONENT(result_source_diagnostic)
            ASN_COMPONENT(responding_AP_title)
            ASN_COMPONENT(responding_AE_qualifier)
            ASN_COMPONENT(responding_AP_invocation_id)
            ASN_COMPONENT(responding_AE_invocation_id)
            ASN_COMPONENT(responder_acse_requirements)
            ASN_COMPONENT(mechanism_name)
            ASN_COMPONENT(responding_authentication_value)
            ASN_COMPONENT(implementation_information)
            ASN_COMPONENT(user_information)
        ASN_END_COMPONENTS
    }
    
    AARE::~AARE()
    {
    }

}