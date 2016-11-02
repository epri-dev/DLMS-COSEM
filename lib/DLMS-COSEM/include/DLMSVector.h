#pragma once

#include <vector>
#include <cstdint>
#include <cstddef>
#include <string>
#include <type_traits>

#include "mapbox/variant.hpp"

using namespace mapbox::util;

namespace EPRI
{
    struct blank
    {};
    
    class DLMSVector;
    
    using DLMSVariantInitList = std::initializer_list<uint32_t>;
    using DLMSVariant = variant<blank, bool, int8_t, uint8_t, int16_t, uint16_t, int32_t, uint32_t, int64_t, uint64_t, 
        std::string, float , double, DLMSVector, DLMSVariantInitList>;
    enum DLMSVariantIndex
    {
        VAR_BLANK     = 0,
        VAR_BOOL      = 1,
        VAR_INT8      = 2,
        VAR_UINT8     = 3,
        VAR_INT16     = 4,
        VAR_UINT16    = 5,
        VAR_INT32     = 6,
        VAR_UINT32    = 7,
        VAR_INT64     = 8,
        VAR_UINT64    = 9,
        VAR_STRING    = 10,
        VAR_FLOAT     = 11,
        VAR_DOUBLE    = 12,
        VAR_VECTOR    = 13,
        VAR_INIT_LIST = 14
    };
    
    bool IsValueInVariant(const DLMSVariant& Value, const DLMSVariant& Variant);

    class DLMSVector
    {
    public:
        DLMSVector();
        DLMSVector(size_t Size);
        DLMSVector(const std::initializer_list<uint8_t>& Value);
        DLMSVector(const DLMSVector& Value);
        ~DLMSVector();

        size_t Size() const;
        size_t GetReadPosition() const;
        bool SetReadPosition(size_t value);
        bool Zero(size_t Position = 0, size_t Count = 0);
        
        template <typename _VariantType, uint8_t BitsToAppend = 0>
            size_t Append(_VariantType Value, bool BigEndian = true)
            {
                static_assert(BitsToAppend == 0 || BitsToAppend == 8 || BitsToAppend == 16 || 
                              BitsToAppend == 32 || BitsToAppend == 64,
                    "Bits must be 0, 8, 16, 32, or 64");
                static_assert(std::is_integral<_VariantType>::value,
                    "_VariantType must be an integral type");

                typedef typename 
                    std::conditional
                    <
                        BitsToAppend == 0, 
                        _VariantType,
                        typename std::conditional
                        <
                            BitsToAppend == 8, 
                            uint8_t,
                            typename std::conditional
                            <
                                BitsToAppend == 16, 
                                uint16_t,
                                typename std::conditional
                                <
                                    BitsToAppend == 32, 
                                    uint32_t,
                                    typename std::conditional
                                    <
                                        BitsToAppend == 64, 
                                        uint64_t,
                                        void
                                    >::type
                                >::type
                            >::type
                        >::type
                    >::type AppendBaseType;
                
                static_assert(sizeof(_VariantType) >= sizeof(AppendBaseType),
                    "Variant type is too small");

                size_t RetVal = m_Data.size();
                for (int Index = 0; Index < sizeof(AppendBaseType); ++Index)
                {
                    if (BigEndian)
                    {
                        m_Data.push_back(0xFF & (Value >> ((sizeof(AppendBaseType) - Index - 1) * 8)));
                    }
                    else
                    {
                        m_Data.push_back(0xFF & (Value >> (Index * 8)));
                    }
                }
                return RetVal;
            }
        
        size_t AppendFloat(float Value);
        size_t AppendDouble(double Value);
        size_t AppendBuffer(const void * pValue, size_t Count);
        ssize_t Append(const DLMSVector& Value, size_t Position = 0, size_t Count = 0);
        size_t Append(const std::string& Value);
        size_t Append(const std::vector<uint8_t>& Value);
        size_t Append(const DLMSVariant& Value, bool Trim = true);
        void Clear();
        template <typename _VariantType, uint8_t BitsToGet = 0>
            bool Get(DLMSVariant * pValue, bool BigEndian = true)
            {
                size_t BytesPeeked = 0;
                bool   RetVal = Peek<_VariantType, BitsToGet>(pValue, BigEndian, 0, &BytesPeeked);
                if (RetVal)
                {
                    m_ReadPosition += BytesPeeked;
                }
                return RetVal;
            }
        bool GetBuffer(uint8_t * pValue, size_t Count);
        bool GetVector(std::vector<uint8_t> * pValue, size_t Count);
        std::vector<uint8_t> GetBytes() const;
        
        int PeekByte(size_t OffsetFromGetPosition = 0) const;
        bool PeekBuffer(uint8_t * pValue, size_t Count) const;
       
        template <typename _VariantType, uint8_t BitsToPeek = 0>
            bool Peek(DLMSVariant * pValue, bool BigEndian = true, size_t Offset = 0, size_t * pBytesPeeked = nullptr) const
            {
                static_assert(BitsToPeek == 0 || BitsToPeek == 8 || BitsToPeek == 16 || 
                              BitsToPeek == 32 || BitsToPeek == 64,
                    "Bits must be 0, 8, 16, 32, or 64");
                static_assert(std::is_integral<_VariantType>::value ||
                    std::is_same<_VariantType, float>::value ||
                    std::is_same<_VariantType, double>::value,
                    "_VariantType must be an integral type, float or double");

                typedef typename 
                    std::conditional
                    <
                        BitsToPeek == 0, 
                        typename std::conditional
                        <
                            std::is_same<_VariantType, float>::value,
                            uint32_t,
                            typename std::conditional
                            <   
                                std::is_same<_VariantType, double>::value,
                                uint64_t,
                                _VariantType
                            >::type
                        >::type,
                        typename std::conditional
                        <
                            BitsToPeek == 8, 
                            uint8_t,
                            typename std::conditional
                            <
                                BitsToPeek == 16, 
                                uint16_t,
                                typename std::conditional
                                <
                                    BitsToPeek == 32, 
                                    uint32_t,
                                    typename std::conditional
                                    <
                                        BitsToPeek == 64, 
                                        uint64_t,
                                        void
                                    >::type
                                >::type
                            >::type
                        >::type
                    >::type PeekBaseType;
                
                static_assert(sizeof(_VariantType) >= sizeof(PeekBaseType),
                    "Variant type is too small");
                    
                if (m_ReadPosition + sizeof(PeekBaseType) + Offset <= m_Data.size())
                {
                    PeekBaseType V = 0;
                    for (int Index = 0; Index < sizeof(PeekBaseType); ++Index)
                    {
                        if (BigEndian)
                        {
                            V |= (PeekBaseType(m_Data[m_ReadPosition + Offset + Index]) << ((sizeof(PeekBaseType) - Index - 1) * 8));
                        }
                        else
                        {
                            V |= (PeekBaseType(m_Data[m_ReadPosition + Offset + 
                                    (sizeof(PeekBaseType) - Index) - 1]) << ((sizeof(PeekBaseType) - Index - 1) * 8));
                        }
                    }
                    pValue->set<_VariantType>(*((_VariantType *)&V));
                    if (nullptr != pBytesPeeked)
                    {
                        *pBytesPeeked = sizeof(PeekBaseType);
                    }
                    return true;
                }
                return false;
            }
        
        std::string ToString();

        DLMSVector& operator=(DLMSVector& lval);
        uint8_t& operator[](size_t Index);
        const uint8_t& operator[](size_t Index) const;
        
    private:
        using RawData = std::vector<uint8_t>;
        RawData                 m_Data;
        size_t                  m_ReadPosition = 0;
    };

}
