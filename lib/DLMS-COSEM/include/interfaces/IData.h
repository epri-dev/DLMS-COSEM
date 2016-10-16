#pragma once

#include "IDLMSInterface.h"

namespace EPRI
{
    class IData_0 : public IDLMSInterface<1, 0>
    {
    public:
        BEGIN_SCHEMA(DataValueSchema)
	        BEGIN_CHOICE
		        NULL_DATA_TYPE
		        BOOLEAN_TYPE
	        END_CHOICE
        END_SCHEMA
            
        virtual ~IData_0()
        {
        }
        
        enum Attributes
        {
            ATTR_VALUE = 2
        };
        
        DLMSAttribute<ATTR_VALUE, DataValueSchema, 0x08> value;
        
    };
    
    template <uint32_t OID>
        class IDataObject : public IData_0, public IDLMSObject<OID>
        {
        
        };

}