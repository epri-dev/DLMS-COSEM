#pragma once

#include "ICOSEMInterface.h"

namespace EPRI
{
    const uint16_t CLASS_ID_CLOCK = 8;
    
    class IClock_0 : public ICOSEMInterface
    {
        COSEM_DEFINE_SCHEMA(Clock_Base_Schema)

    public :
        IClock_0();
        virtual ~IClock_0();
        
        enum Attributes : ObjectAttributeIdType
        {
            ATTR_TIME          = 2,
            ATTR_TIME_ZONE     = 3,
            ATTR_STATUS        = 4,
            ATTR_DST_BEGIN     = 5,
            ATTR_DST_END       = 6,
            ATTR_DST_DEVIATION = 7,
            ATTR_DST_ENABLED   = 8,
            ATTR_CLOCK_BASE    = 9
        };
        
        enum ClockBase : uint8_t
        {
            NOT_DEFINED = 0,
            INTERNAL_CRYSTAL = 1,
            MAINS_50HZ = 2,
            MAINS_60HZ = 3,
            GPS = 4,
            RADIO = 5
        };
        
        COSEMAttribute<ATTR_TIME, OctetStringSchema, 0x08>       time;
        COSEMAttribute<ATTR_TIME_ZONE, LongSchema, 0x10>         time_zone;
        COSEMAttribute<ATTR_STATUS, UnsignedSchema, 0x18>        status;
        COSEMAttribute<ATTR_DST_BEGIN, OctetStringSchema, 0x20>  daylight_savings_begin;
        COSEMAttribute<ATTR_DST_END, OctetStringSchema, 0x28>    daylight_savings_end;
        COSEMAttribute<ATTR_DST_DEVIATION, IntegerSchema, 0x30>  daylight_savings_deviation;
        COSEMAttribute<ATTR_DST_ENABLED, BooleanSchema, 0x38>    daylight_savings_enabled;
        COSEMAttribute<ATTR_CLOCK_BASE, Clock_Base_Schema, 0x40> clock_base;
//        
//        BEGIN_SCHEMA(AdjustingTimeSchema)
//            STRUCTURE_TYPE
//                OCTET_STRING_TYPE
//                OCTET_STRING_TYPE
//                OCTET_STRING_TYPE
//            END_STRUCTURE_TYPE
//        END_SCHEMA
//        
//        DLMSMethod<EmptySchema, IntegerSchema, 0x60>            adjust_to_quarter;
//        DLMSMethod<EmptySchema, IntegerSchema, 0x68>            adjust_to_measuring_period;
//        DLMSMethod<EmptySchema, IntegerSchema, 0x70>            adjust_to_minute;
//        DLMSMethod<EmptySchema, IntegerSchema, 0x78>            adjust_to_preset_time;
//        DLMSMethod<EmptySchema, AdjustingTimeSchema, 0x80>      preset_adjusting_time;
//        DLMSMethod<EmptySchema, LongSchema, 0x88>               shift_time;
        
    };
    
    class IClockObject : public IClock_0, public ICOSEMObject
    {
    public:
        IClockObject() = delete;
        IClockObject(const COSEMObjectInstanceCriteria& OIDCriteria, 
            uint16_t ShortNameBase = std::numeric_limits<uint16_t>::max());
        virtual ~IClockObject();
    };

}