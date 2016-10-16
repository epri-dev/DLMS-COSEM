#pragma once

// OBIS

#include "DLMSDataTypes.h"
#include "DLMSData.h"
#include "DLMSAttribute.h"
#include "DLMSMethod.h"

namespace EPRI
{
    enum DLMSInternalDataType : int16_t
    {
        END_STRUCTURE_T     = -DLMSDataType::STRUCTURE,
        END_ARRAY_T         = -DLMSDataType::ARRAY,
        END_COMPACT_ARRAY_T = -DLMSDataType::COMPACT_ARRAY,
        BEGIN_CHOICE_T      = -256,
        END_CHOICE_T        = -257,
    };

#define BEGIN_SCHEMA(SDEF) \
        constexpr int16_t SDEF[] =\
        { 
#define BEGIN_CHOICE\
            EPRI::DLMSInternalDataType::BEGIN_CHOICE_T,
#define END_CHOICE\
            EPRI::DLMSInternalDataType::END_CHOICE_T,
#define NULL_DATA_TYPE\
            EPRI::DLMSDataType::NULL_DATA,
#define OCTET_STRING_TYPE\
            EPRI::DLMSDataType::OCTET_STRING,
#define LONG_TYPE\
            EPRI::DLMSDataType::LONG,
#define UNSIGNED_TYPE\
            EPRI::DLMSDataType::UNSIGNED,
#define INTEGER_TYPE\
            EPRI::DLMSDataType::INTEGER,
#define BOOLEAN_TYPE\
            EPRI::DLMSDataType::BOOLEAN,
#define ENUM_TYPE\
            EPRI::DLMSDataType::ENUM,
#define STRUCTURE_TYPE\
            EPRI::DLMSDataType::STRUCTURE,
#define END_STRUCTURE_TYPE\
            EPRI::DLMSInternalDataType::END_STRUCTURE_T,
#define END_SCHEMA\
        };
        
    // 
    // Default Basic Schemas
    //
    BEGIN_SCHEMA(EmptySchema)
    END_SCHEMA
    BEGIN_SCHEMA(OctetStringSchema)
        OCTET_STRING_TYPE
    END_SCHEMA
    BEGIN_SCHEMA(LongSchema)
        LONG_TYPE
    END_SCHEMA
    BEGIN_SCHEMA(UnsignedSchema)
        UNSIGNED_TYPE
    END_SCHEMA
    BEGIN_SCHEMA(IntegerSchema)
        INTEGER_TYPE
    END_SCHEMA
    BEGIN_SCHEMA(BooleanSchema)
        BOOLEAN_TYPE
    END_SCHEMA
    BEGIN_SCHEMA(EnumSchema)
        BOOLEAN_TYPE
    END_SCHEMA
    
    template <uint16_t class_id, uint8_t version, 
        uint16_t CardinalityMin = 0, 
        uint16_t CardinalityMax = std::numeric_limits<uint16_t>::max()>
        class IDLMSInterface
        {
        public:
               
            enum Attributes : AttributeIDType
            {
                ATTRIBUTE_0  = 0,
                LOGICAL_NAME = 1
            };
                
            DLMSAttribute<LOGICAL_NAME, OctetStringSchema, 0x00> logical_name;
            
        protected:
            const uint16_t m_class_id = class_id;
            const uint8_t m_version = version;
            const uint16_t m_CardinalityMin = CardinalityMin;
            const uint16_t m_CardinalityMax = CardinalityMax;
            
            static uint16_t m_CardinalityCounter;
            
        };
    
    template <uint32_t OID, uint16_t ShortNameBase = std::numeric_limits<uint16_t>::max()>
        class IDLMSObject
        {
        
        };
}