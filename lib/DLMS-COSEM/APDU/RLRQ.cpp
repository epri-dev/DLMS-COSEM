#include "APDU/RLRQ.h"

namespace EPRI
{

    ASN_BEGIN_SCHEMA(RLRQ::Release_request_reason_Schema)
        ASN_INTEGER_LIST_TYPE
        (   
            ASN::IMPLICIT, 
            { 
                normal, 
                urgent, 
                user_defined
            }
        )
    ASN_END_SCHEMA
                
    RLRQ::RLRQ()
    {
        ASN_BEGIN_COMPONENTS
            ASN_COMPONENT(reason)
            ASN_COMPONENT(user_information)
        ASN_END_COMPONENTS
    }
    
    RLRQ::~RLRQ()
    {
    }

}