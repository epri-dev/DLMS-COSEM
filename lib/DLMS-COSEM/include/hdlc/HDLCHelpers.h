#pragma once

#include <cstdint>

namespace EPRI
{
    uint16_t Get16BigEndianHelper(const uint8_t * pBuffer);
    uint16_t Get32BigEndianHelper(const uint8_t * pBuffer);
}
