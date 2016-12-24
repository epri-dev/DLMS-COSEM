#pragma once

#include "APDU.h"
#include "APDUConstants.h"
#include "COSEMTypes.h"

namespace EPRI
{
    using Set_Response_Base = APDUSingleType<197>;
        
    class Set_Response : public Set_Response_Base
    {
        ASN_DEFINE_SCHEMA(Set_Response_Schema)
            
    public :
        Set_Response();
        virtual ~Set_Response();
        
        enum Set_Response_Choice : int8_t
        {
            set_response_normal                        = 1,
            set_response_with_first_datablock          = 2,
            set_response_with_datablock                = 3,
            set_response_with_list                     = 4,
            set_response_with_list_and_first_datablock = 5
        };
        
        virtual bool IsValid() const;
        
    };
    
    class Set_Response_Normal : public Set_Response
    {
    public:
        Set_Response_Normal();
        virtual ~Set_Response_Normal();
        //
        // Attributes
        //
        InvokeIdAndPriorityType           invoke_id_and_priority;
        APDUConstants::Data_Access_Result result;
        //
        virtual bool Parse(DLMSVector * pData,
            COSEMAddressType SourceAddress,
            COSEMAddressType DestinationAddress);
        virtual std::vector<uint8_t> GetBytes();
        
    };

}