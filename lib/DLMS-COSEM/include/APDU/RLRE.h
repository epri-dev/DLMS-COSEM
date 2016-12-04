#pragma once

#include "APDU.h"
#include "APDUConstants.h"

namespace EPRI
{

    class RLRE : public APDU<0x63>
    {
        ASN_DEFINE_SCHEMA(Release_response_reason_Schema)

    public :
        RLRE();
        virtual ~RLRE();

        APDUComponent<0, Release_response_reason_Schema, 
                      ASN::IMPLICIT | ASN::OPTIONAL>        reason;
        APDU_Association_information<30>                    user_information;
        
        enum ReleaseResponseReason : int8_t
        {
            normal             = 0,
            not_finished       = 1,
            user_defined       = 30
        };
    };

}