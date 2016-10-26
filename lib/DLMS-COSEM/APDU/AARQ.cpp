#include "APDU/AARQ.h"

namespace EPRI
{
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