#include "APDU/AARE.h"

namespace EPRI
{

    ASN_BEGIN_SCHEMA(AARE::Association_Result_Schema)
        ASN_INTEGER_LIST_TYPE
        (   
            ASN::NO_OPTIONS, 
            { 
                accepted, 
                rejected_permanent, 
                rejected_transient,
            }
        )
    ASN_END_SCHEMA
    ASN_BEGIN_SCHEMA(AARE::Associate_Source_Diagnostic_Schema)
        ASN_BEGIN_CHOICE
            ASN_BEGIN_CHOICE_ENTRY_WITH_OPTIONS(acse_service_user, ASN::CONSTRUCTED)
                ASN_INTEGER_LIST_TYPE
                (   
                    ASN::CONSTRUCTED, 
                    { 
                        user_null, 
                        user_no_reason_given, 
                        application_context_name_not_supported,
                        authentication_mechanism_name_not_recognized,
                        authentication_mechanism_name_required,
                        authentication_failure,
                        authentication_required,
                    }
                )
            ASN_END_CHOICE_ENTRY
            ASN_BEGIN_CHOICE_ENTRY_WITH_OPTIONS(acse_service_provider, ASN::CONSTRUCTED)
                ASN_INTEGER_LIST_TYPE
                (   
                    ASN::CONSTRUCTED, 
                    { 
                        provider_null, 
                        provider_no_reason_given, 
                        no_common_acse_version,
                    }
                )
            ASN_END_CHOICE_ENTRY
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