#pragma once

#include "APDU.h"
#include "APDUConstants.h"

namespace EPRI
{
    using Get_Response_Base = APDUSingleType<196>;
        
    class Get_Response : public Get_Response_Base
    {
        ASN_DEFINE_SCHEMA(Get_Response_Schema)
            
    public :
        Get_Response();
        virtual ~Get_Response();
        
        enum Get_Response_Choice : int8_t
        {
            get_response_normal    = 1,
            get_response_next      = 2,
            get_response_with_list = 3
        };
        
        enum Get_Data_Result_Choice : int8_t
        {
            data = 0,
            data_access_result = 1
        };
        typedef variant<DLMSVector, APDUConstants::Data_Access_Result> Get_Data_Result;
        
        virtual bool IsValid() const;
        
    };
    
    class Get_Response_Normal : public Get_Response
    {
    public:
        Get_Response_Normal();
        virtual ~Get_Response_Normal();
        //
        // Attributes
        //
        uint8_t          invoke_id_and_priority;
        Get_Data_Result  result;
        //
        virtual bool Parse(DLMSVector * pData);
        virtual std::vector<uint8_t> GetBytes();
        
    };

}