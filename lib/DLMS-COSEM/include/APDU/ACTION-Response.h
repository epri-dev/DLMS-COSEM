#pragma once

#include "APDU.h"
#include "APDUConstants.h"
#include "COSEMTypes.h"

namespace EPRI
{
    using Action_Response_Base = APDUSingleType<199>;
        
    class Action_Response : public Action_Response_Base
    {
        ASN_DEFINE_SCHEMA(Action_Response_Schema)
            
    public :
        Action_Response();
        virtual ~Action_Response();
        
        enum Action_Response_Choice : int8_t
        {
            action_response_normal       = 1,
            action_response_with_pblock  = 2,
            action_response_with_list    = 3,
            action_response_next_pblock  = 4
        };
        
        virtual bool IsValid() const;
        
    };
    
    struct Action_Response_With_Optional_Data
    {
        APDUConstants::Action_Result   result;
        DLMSOptional<Get_Data_Result>  return_parameters;
    };
    
    class Action_Response_Normal : public Action_Response
    {
    public:
        Action_Response_Normal();
        virtual ~Action_Response_Normal();
        //
        // Attributes
        //
        InvokeIdAndPriorityType            invoke_id_and_priority;
        Action_Response_With_Optional_Data single_response;
        //
        virtual bool Parse(DLMSVector * pData,
            COSEMAddressType SourceAddress,
            COSEMAddressType DestinationAddress);
        virtual std::vector<uint8_t> GetBytes();
        
    };

}