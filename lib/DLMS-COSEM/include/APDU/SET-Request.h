#pragma once

#include "APDU.h"
#include "APDUConstants.h"
#include "COSEMTypes.h"

namespace EPRI
{
    using Set_Request_Base = APDUSingleType<193>;
        
    class Set_Request : public Set_Request_Base
    {
        ASN_DEFINE_SCHEMA(Set_Request_Schema)
            
    public :
        Set_Request();
        virtual ~Set_Request();
        
        enum Set_Request_Choice : int8_t
        {
            set_request_normal                        = 1,
            set_request_with_first_datablock          = 2,
            set_request_with_datablock                = 3,
            set_request_with_list                     = 4,
            set_request_with_list_and_first_datablock = 5
        };
        //
        // Attributes
        //
        InvokeIdAndPriorityType invoke_id_and_priority;
        
        virtual bool IsValid() const;
        
    };
    
    class Set_Request_Normal : public Set_Request
    {
    public:
        Set_Request_Normal();
        virtual ~Set_Request_Normal();
        //
        // Attributes
        //
        Cosem_Attribute_Descriptor  cosem_attribute_descriptor;
        DLMSVector                  value;
        
        virtual bool Parse(DLMSVector * pData,
            COSEMAddressType SourceAddress,
            COSEMAddressType DestinationAddress);
        virtual std::vector<uint8_t> GetBytes();
        
    };

}