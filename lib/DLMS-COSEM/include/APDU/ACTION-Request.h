#pragma once

#include "APDU.h"
#include "APDUConstants.h"
#include "COSEMTypes.h"

namespace EPRI
{
    using Action_Request_Base = APDUSingleType<195>;
        
    class Action_Request : public Action_Request_Base
    {
        ASN_DEFINE_SCHEMA(Action_Request_Schema)
            
    public :
        Action_Request();
        virtual ~Action_Request();
        
        enum Action_Request_Choice : int8_t
        {
            action_request_normal                       = 1,
            action_request_next_pblock                  = 2,
            action_request_with_list                    = 3,
            action_request_with_first_pblock            = 4,
            action_request_with_list_and_first_pblock   = 5,
            action_request_with_pblock                  = 6
        };
        //
        // Attributes
        //
        InvokeIdAndPriorityType invoke_id_and_priority;
        
        virtual bool IsValid() const;
        
    };
    
    class Action_Request_Normal : public Action_Request
    {
    public:
        Action_Request_Normal();
        virtual ~Action_Request_Normal();
        //
        // Attributes
        //
        Cosem_Method_Descriptor     cosem_method_descriptor;
        DLMSOptional<DLMSVector>    method_invocation_parameters;
        
        virtual bool Parse(DLMSVector * pData,
            COSEMAddressType SourceAddress,
            COSEMAddressType DestinationAddress);
        virtual std::vector<uint8_t> GetBytes();
        
    };

}