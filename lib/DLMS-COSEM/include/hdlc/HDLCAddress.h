#pragma once

#include <cstddef>
#include <cstdint>
#include <cstring>

#include "HDLCHelpers.h"
#include "COSEMAddress.h"

namespace EPRI
{
    class HDLCAddress
    {
    public:
        enum ShortReservedAddress : uint8_t
        {
            SHORT_NO_STATION = ReservedAddresses::NO_STATION,
            SHORT_MANAGEMENT = ReservedAddresses::MANAGEMENT,
            SHORT_PUBLIC_CLIENT = ReservedAddresses::PUBLIC_CLIENT,
            SHORT_BROADCAST = ReservedAddresses::BROADCAST,
            SHORT_CALLING = ReservedAddresses::CALLING
        };
        enum LongReservedAddress : uint16_t
        {
            LONG_NO_STATION    = ReservedAddresses::NO_STATION,
            LONG_MANAGEMENT    = ReservedAddresses::MANAGEMENT,
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
