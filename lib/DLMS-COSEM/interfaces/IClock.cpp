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
    //
    // IClock_0
    //
    IClock_0::IClock_0()
        : ICOSEMInterface(CLASS_ID_CLOCK, 0, 0, 1)
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