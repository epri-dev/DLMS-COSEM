#pragma once

#include "APDU.h"
#include "APDUConstants.h"

namespace EPRI
{

    class RLRQ : public APDU<0x62>
    {
        ASN_DEFINE_SCHEMA(Release_request_reason_Schema)

    public:
        RLRQ();
        virtual ~RLRQ();

        APDUComponent<0, Release_request_reason_Schema, 
                      ASN::IMPLICIT | ASN::OPTIONAL>        reason;
        APDU_Association_information<30>                    user_information;

        enum ReleaseRequestReason : int8_t
        {
            normal       = 0,
            urgent       = 1,
            user_defined = 30
        };
        
    };

}