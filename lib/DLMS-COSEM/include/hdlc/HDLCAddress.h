#pragma once

#include <cstddef>
#include <cstdint>
#include <cstring>

#include "HDLCHelpers.h"

namespace EPRI
{
    class HDLCAddress
    {
    public:
        const uint16_t INVALID_ADDRESS = 0xFFFF;
        enum ShortReservedAddress : uint8_t
        {
            SHORT_NO_STATION = 0x00,
            SHORT_PUBLIC_CLIENT = 0x01,
            SHORT_MANAGEMENT = 0x10,
            SHORT_BROADCAST = 0x7F,
            SHORT_CALLING = 0x7E
        };
        enum LongReservedAddress : uint16_t
        {
            LONG_NO_STATION    = 0x0000,
            LONG_MANAGEMENT    = 0x0001,
            LONG_BROADCAST     = 0x3FFF,
            LONG_CALLING       = 0x3FFE
        };
       
        HDLCAddress();
        HDLCAddress(uint8_t LogicalAddress);
        HDLCAddress(uint8_t LogicalAddressUpper, uint8_t PhysicalAddressLower);
        HDLCAddress(uint16_t LogicalAddressUpper, uint16_t PhysicalAddressLower);
        HDLCAddress(const HDLCAddress& Src);
        ~HDLCAddress();

        uint16_t LogicalAddress() const;
        uint16_t PhysicalAddress() const;
        void Copy(const HDLCAddress& Src);
        void Clear();

        HDLCAddress& operator=(const HDLCAddress& Src);
        bool operator ==(const HDLCAddress &b) const;
        HDLCAddress& Parse(const uint8_t * pAddressFromPHY);

        inline size_t Size() const
        {
            return m_Size;
        }
        inline bool IsEmpty() const
        {
            return 0 == m_Size;
        }
        inline const uint8_t * Get() const
        {
            return m_Address;
        }
        inline const uint8_t * operator&()
        {
            return m_Address;
        }
        
    protected:
        inline uint8_t GetAdjustedByte(int Index) const
        {
            return (m_Address[Index] & 0xFE) >> 1;
        }
        size_t  m_Size = 0;
        uint8_t m_Address[sizeof(uint16_t) + sizeof(uint16_t)] = { };
    };
    
}
