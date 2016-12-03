#pragma once

#include <cstdint>

namespace EPRI
{
    enum ReservedAddresses : uint8_t
    {
        NO_STATION    = 0x00,
        MANAGEMENT    = 0x01,
        PUBLIC_CLIENT = 0x10,
        BROADCAST     = 0x7F,
        CALLING       = 0x7E
    };
    typedef uint16_t COSEMAddressType; 
    
    const COSEMAddressType INVALID_ADDRESS = 0xFFFF;

}
