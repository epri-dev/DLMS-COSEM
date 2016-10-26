#pragma once

#include <cstdint>

namespace EPRI
{
    struct HDLCOptions
    {
        bool     StartWithIEC;
        uint16_t NumberOfRetries;
        uint32_t InterOctetTimeoutInMs;
    };
}