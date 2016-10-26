#pragma once

#include "APDU.h"
#include "APDUConstants.h"

namespace EPRI
{

    class AARQ : public APDU<0x60>
    {
    public:
        AARQ();
        virtual ~AARQ();
        
        APDU_Protocol_Version<0>         protocol_version;
        APDU_application_context_name<1> application_context_name;
        APDU_AP_title<2>                 called_AP_title;
        APDU_AE_qualifier<3>             called_AE_qualifier;
        APDU_AP_invocation_identifier<4> called_AP_invocation_id;
        APDU_AE_invocation_identifier<5> called_AE_invocation_id;
        APDU_AP_title<6>                 calling_AP_title;
        APDU_AE_qualifier<7>             calling_AE_qualifier;
        APDU_AP_invocation_identifier<8> calling_AP_invocation_id;
        APDU_AE_invocation_identifier<9> calling_AE_invocation_id;
        APDU_ACSE_Requirements<10>       sender_acse_requirements;
        APDU_Mechanism_name<11>          mechanism_name;
        APDU_Authentication_Value<12>    calling_authentication_value;
        APDU_Implementation_data<29>     implementation_information;
        APDU_Association_information<30> user_information;
    };

}