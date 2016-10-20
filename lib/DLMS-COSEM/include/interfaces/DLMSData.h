#pragma once

#include <cstdint>
#include <vector>

namespace EPRI
{
    enum DLMSDataType : uint8_t
    {
        NULL_DATA            = 0,
        ARRAY                = 1,
        STRUCTURE            = 2,
        BOOLEAN              = 3,
        BIT_STRING           = 4,
        DOUBLE_LONG          = 5,
        DOUBLE_LONG_UNSIGNED = 6,
        FLOATING_POINT       = 7,
        OCTET_STRING         = 9,
        VISIBLE_STRING       = 10,
        BCD                  = 13,
        INTEGER              = 15,
        LONG                 = 16,
        UNSIGNED             = 17,
        LONG_UNSIGNED        = 18,
        COMPACT_ARRAY        = 19,
        LONG64               = 20,
        LONG64_UNSIGNED      = 21,
        ENUM                 = 22,
        FLOAT32              = 23,
        FLOAT64              = 24,
        DATE_TIME            = 25,
        DATE                 = 26,
        TIME                 = 27,
        DONT_CARE            = 255
    };
    
}
