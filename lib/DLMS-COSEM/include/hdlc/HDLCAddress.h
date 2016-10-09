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
       
        HDLCAddress()
        {
        }
        HDLCAddress(uint8_t LogicalAddress) :
            m_Size(sizeof(uint8_t))
        {
            m_Address[0] = (LogicalAddress << 1) | 0x01;
        }
        HDLCAddress(uint8_t LogicalAddressUpper, uint8_t PhysicalAddressLower) : 
            m_Size(2 * sizeof(uint8_t))
        {
            m_Address[0] = (LogicalAddressUpper << 1);
            m_Address[1] = (PhysicalAddressLower << 1) | 0x01;
        }
        HDLCAddress(uint16_t LogicalAddressUpper, uint16_t PhysicalAddressLower) : 
            m_Size(2 * sizeof(uint16_t))
        {
            m_Address[1] = uint8_t(LogicalAddressUpper & 0x0000007F) << 1;
            LogicalAddressUpper >>= 7;
            m_Address[0] = uint8_t(LogicalAddressUpper & 0x0000007F) << 1;
            LogicalAddressUpper >>= 7;
            m_Address[3] = uint8_t(PhysicalAddressLower & 0x0000007F) << 1;
            PhysicalAddressLower >>= 7;
            m_Address[2] = uint8_t(PhysicalAddressLower & 0x0000007F) << 1;
            PhysicalAddressLower >>= 7;
            m_Address[3] |= 0x01;
        }
        HDLCAddress(const HDLCAddress& Src)
        {
            Copy(Src);
        }
        ~HDLCAddress()
        {
        }
        
        HDLCAddress& operator=(const HDLCAddress& Src)
        {
            Copy(Src);
            return *this;
        }
        
        bool operator ==(const HDLCAddress &b) const
        {
            return (m_Size == b.m_Size) &&
                (std::memcmp(m_Address, b.m_Address, m_Size) == 0);
        }
        
        HDLCAddress& Parse(const uint8_t * pAddressFromPHY)
        {
            int Index = 0;
            for (; Index < (2 * sizeof(uint16_t)); Index++)
            {
                m_Address[Index] = *pAddressFromPHY++;
                if (m_Address[Index] & 0x01)
                {
                    break;
                }
            }
            m_Size = Index + 1;
            return *this;
        }

        inline size_t Size() const
        {
            return m_Size;
        }
        
        const uint8_t * Get() const
        {
            return m_Address;
        }
        
        const uint8_t * operator&()
        {
            return m_Address;
        }
        
        uint16_t LogicalAddress() const
        {
            switch (Size())
            {
            case 1:
            case 2:
                return GetAdjustedByte(0);
            case 4:
                return uint16_t(GetAdjustedByte(0) << 8) | uint16_t(GetAdjustedByte(1));
            }
            return INVALID_ADDRESS;
        }
        
        uint16_t PhysicalAddress() const
        {
            switch (Size())
            {
            case 1:
                return INVALID_ADDRESS;
            case 2:
                return GetAdjustedByte(1);
            case 4:
                return uint16_t(GetAdjustedByte(2) << 8) | uint16_t(GetAdjustedByte(3));
            }
            return INVALID_ADDRESS;
        }
        
        void Copy(const HDLCAddress& Src)
        {
            std::memcpy(m_Address, Src.m_Address, Src.m_Size);
            m_Size = Src.m_Size;
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
