#include "interfaces/IClock.h"

namespace EPRI
{
    COSEM_BEGIN_SCHEMA(IClock_0::Clock_Base_Schema)
        COSEM_ENUM_TYPE
        (   
            { 
                IClock_0::NOT_DEFINED, 
                IClock_0::INTERNAL_CRYSTAL, 
                IClock_0::MAINS_50HZ,
                IClock_0::MAINS_60HZ,
                IClock_0::GPS,
                IClock_0::RADIO
            }
        )
    COSEM_END_SCHEMA
        
    COSEM_BEGIN_SCHEMA(IClock_0::Adjusting_Time_Schema)
        COSEM_BEGIN_STRUCTURE
            COSEM_OCTET_STRING_TYPE
            COSEM_OCTET_STRING_TYPE
            COSEM_OCTET_STRING_TYPE
        COSEM_END_STRUCTURE
    COSEM_END_SCHEMA
    //
    // IClock_0
    //
    IClock_0::IClock_0()
        : ICOSEMInterface(CLSID_IClock, 0, 0, 1)
    {
        COSEM_BEGIN_ATTRIBUTES
            COSEM_ATTRIBUTE(time)
            COSEM_ATTRIBUTE(time_zone)
            COSEM_ATTRIBUTE(status)
            COSEM_ATTRIBUTE(daylight_savings_begin)
            COSEM_ATTRIBUTE(daylight_savings_end)
            COSEM_ATTRIBUTE(daylight_savings_deviation)
            COSEM_ATTRIBUTE(daylight_savings_enabled)
            COSEM_ATTRIBUTE(clock_base)
        COSEM_END_ATTRIBUTES
            
        COSEM_BEGIN_METHODS
            COSEM_METHOD(adjust_to_quarter)
            COSEM_METHOD(adjust_to_measuring_period)
            COSEM_METHOD(adjust_to_minute)
            COSEM_METHOD(adjust_to_preset_time)
            COSEM_METHOD(preset_adjusting_time)
            COSEM_METHOD(shift_time)
        COSEM_END_METHODS         
    }
        
    IClock_0::~IClock_0()
    {
    }
    //
    // IClockObject
    //
    IClockObject::IClockObject(const COSEMObjectInstanceCriteria& OIDCriteria, 
        uint16_t ShortNameBase /* = std::numeric_limits<uint16_t>::max() */)
        : ICOSEMObject(OIDCriteria, ShortNameBase)
    {
    }
        
    IClockObject::~IClockObject()
    {
    }
    
}