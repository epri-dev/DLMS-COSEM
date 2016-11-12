#pragma once

#include "APDU.h"
#include "APDUConstants.h"

namespace EPRI
{
    using Get_Request_Base = APDUSingleType<192>;
        
    class Get_Request : public Get_Request_Base
    {
        ASN_DEFINE_SCHEMA(Get_Request_Schema)
            
    public :
        Get_Request();
        virtual ~Get_Request();
        
        enum Get_Request_Choice : int8_t
        {
            get_request_normal    = 1,
            get_request_next      = 2,
            get_request_with_list = 3
        };
        
        virtual bool IsValid() const;
        
    };
    
    class Get_Request_Normal : public Get_Request
    {
    public:
        Get_Request_Normal();
        virtual ~Get_Request_Normal();
        
        struct Cosem_Attribute_Descriptor
        {
            uint16_t    class_id;
            DLMSVector  instance_id;
            int8_t      attribute_id;
        };
       
        uint8_t                     invoke_id_and_priority;
        Cosem_Attribute_Descriptor  cosem_attribute_descriptor;
        
        virtual bool Parse(DLMSVector * pData);
        virtual std::vector<uint8_t> GetBytes();
        
    };

}