#pragma once

#include <cstdint>

namespace EPRI
{
    struct HDLCOptions
    {
        bool     StartWithIEC;
        uint16_t NumberOfRetries;
        uint32_t InterOctetTimeoutInMs;
        
        HDLCOptions(bool IEC = false, uint16_t Retries = 3, uint32_t IOTOMs = 500)
            : StartWithIEC(IEC)
            , NumberOfRetries(Retries)
            , InterOctetTimeoutInMs(IOTOMs)
        {
        }
    };
}