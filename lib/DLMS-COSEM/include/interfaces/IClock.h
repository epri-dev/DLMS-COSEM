#pragma once

#include "ICOSEMInterface.h"

namespace EPRI
{
    const uint16_t CLSID_IClock = 8;
    
    class IClock_0 : public ICOSEMInterface
    {
        COSEM_DEFINE_SCHEMA(Clock_Base_Schema)
        COSEM_DEFINE_SCHEMA(Adjusting_Time_Schema)

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
        
        COSEMAttribute<ATTR_TIME, OctetStringSchema, 0x08>                     time;
        COSEMAttribute<ATTR_TIME_ZONE, LongSchema, 0x10>                       time_zone;
        COSEMAttribute<ATTR_STATUS, UnsignedSchema, 0x18>                      status;
        COSEMAttribute<ATTR_DST_BEGIN, OctetStringSchema, 0x20>                daylight_savings_begin;
        COSEMAttribute<ATTR_DST_END, OctetStringSchema, 0x28>                  daylight_savings_end;
        COSEMAttribute<ATTR_DST_DEVIATION, IntegerSchema, 0x30>                daylight_savings_deviation;
        COSEMAttribute<ATTR_DST_ENABLED, BooleanSchema, 0x38>                  daylight_savings_enabled;
        COSEMAttribute<ATTR_CLOCK_BASE, Clock_Base_Schema, 0x40>               clock_base;
        
        enum Methods : ObjectAttributeIdType
        {
            METHOD_ADJUST_TO_QUARTER          = 1,
            METHOD_ADJUST_TO_MEAS_PERIOD      = 2,
            METHOD_ADJUST_TO_MINUTE           = 3,
            METHOD_ADJUST_TO_PRESET_TIME      = 4,
            METHOD_PRESET_ADJUSTING_TIME      = 5,
            METHOD_SHIFT_TIME                 = 6,
        };
        
        COSEMMethod<METHOD_ADJUST_TO_QUARTER, IntegerSchema, 0x60>             adjust_to_quarter;
        COSEMMethod<METHOD_ADJUST_TO_MEAS_PERIOD, IntegerSchema, 0x68>         adjust_to_measuring_period;
        COSEMMethod<METHOD_ADJUST_TO_MINUTE, IntegerSchema, 0x70>              adjust_to_minute;
        COSEMMethod<METHOD_ADJUST_TO_PRESET_TIME, IntegerSchema, 0x78>         adjust_to_preset_time;
        COSEMMethod<METHOD_PRESET_ADJUSTING_TIME, Adjusting_Time_Schema, 0x80> preset_adjusting_time;
        COSEMMethod<METHOD_SHIFT_TIME, LongSchema, 0x88>                       shift_time;
        
    };
    
    typedef IClock_0 IClock;
    
    class IClockObject : public IClock, public ICOSEMObject
    {
    public:
        IClockObject() = delete;
        IClockObject(const COSEMObjectInstanceCriteria& OIDCriteria, 
            uint16_t ShortNameBase = std::numeric_limits<uint16_t>::max());
        virtual ~IClockObject();
        
    protected:
        virtual APDUConstants::Action_Result InternalAction(const AssociationContext& Context,
            ICOSEMMethod * pMethod, 
            const Cosem_Method_Descriptor& Descriptor, 
            const DLMSOptional<DLMSVector>& Parameters,
            DLMSVector * pReturnValue = nullptr) = 0;
        
    };

}