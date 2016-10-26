#pragma once

#include "APDU.h"
#include "APDUConstants.h"

namespace EPRI
{

    class AARE : public APDU<0x61>
    {
        ASN_DEFINE_SCHEMA(Association_Result_Schema)
        ASN_DEFINE_SCHEMA(Associate_Source_Diagnostic_Schema)
            
    public :
        AARE();
        virtual ~AARE();
        
        APDU_Protocol_Version<0>         protocol_version;
        APDU_application_context_name<1> application_context_name;
        APDUComponent<2, Association_Result_Schema, ASN::CONSTRUCTED>
                                         result;
        APDUComponent<3, Associate_Source_Diagnostic_Schema, ASN::CONSTRUCTED>
                                         result_source_diagnostic;
        APDU_AP_title<4>                 responding_AP_title;
        APDU_AE_qualifier<5>             responding_AE_qualifier;
        APDU_AP_invocation_identifier<6> responding_AP_invocation_id;
        APDU_AE_invocation_identifier<7> responding_AE_invocation_id;
        APDU_ACSE_Requirements<8>        responder_acse_requirements;
        APDU_Mechanism_name<9>           mechanism_name;
        APDU_Authentication_Value<10>    responding_authentication_value;
        APDU_Implementation_data<29>     implementation_information;
        APDU_Association_information<30> user_information;
    };

}