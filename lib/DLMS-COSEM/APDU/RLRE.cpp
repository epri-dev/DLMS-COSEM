#include "APDU/RLRE.h"

namespace EPRI
{

    ASN_BEGIN_SCHEMA(RLRE::Release_response_reason_Schema)
        ASN_FIXED_INTEGER_LIST_TYPE
        (   
            ASN::IMPLICIT, 
            sizeof(RLRE::ReleaseResponseReason),
            { 
                normal, 
                not_finished, 
                user_defined
            }
        )
    ASN_END_SCHEMA
                
    RLRE::RLRE()
    {
        ASN_BEGIN_COMPONENTS
            ASN_COMPONENT(reason)
            ASN_COMPONENT(user_information)
        ASN_END_COMPONENTS
    }
    
    RLRE::~RLRE()
    {
    }

}