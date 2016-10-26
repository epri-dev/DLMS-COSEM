#pragma once

#include "APDU.h"

namespace EPRI
{

    class AARQ : public APDU<0x60>
    {
        static const ASNType             protocol_version_default;
        
        ASN_DEFINE_SCHEMA(protocol_version_Schema)
        ASN_DEFINE_SCHEMA(calling_authentication_value_Schema)
        ASN_DEFINE_SCHEMA(sender_acse_requirements_Schema)
            
    public:
        AARQ();
        virtual ~AARQ();
        
        APDUComponent<0, protocol_version_Schema, 
            ASN::IMPLICIT | ASN::OPTIONAL, protocol_version_default> 
                                         protocol_version;
        APDU_application_context_name<1> application_context_name;
        APDU_AP_title<2>                 called_AP_title;
        APDU_AE_qualifier<3>             called_AE_qualifier;
        APDU_AP_invocation_identifier<4> called_AP_invocation_id;
        APDU_AE_invocation_identifier<5> called_AE_invocation_id;
        APDU_AP_title<6>                 calling_AP_title;
        APDU_AE_qualifier<7>             calling_AE_qualifier;
        APDU_AP_invocation_identifier<8> calling_AP_invocation_id;
        APDU_AE_invocation_identifier<9> calling_AE_invocation_id;
        APDUComponent<10, sender_acse_requirements_Schema, ASN::OPTIONAL | ASN::IMPLICIT>
                                         sender_acse_requirements;
        APDU_Mechanism_name<11>          mechanism_name;
        APDUComponent<12, calling_authentication_value_Schema, 
            ASN::CONSTRUCTED | ASN::EXPLICIT | ASN::OPTIONAL>
                                         calling_authentication_value;
        APDU_Implementation_data<29>     implementation_information;
        APDU_Association_information<30> user_information;
    };

}