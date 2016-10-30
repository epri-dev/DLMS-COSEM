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
    
    bool IsValueInVariant(const DLMSVariant& Variant, const DLMSVariant& Value);

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
        size_t AppendUInt8(uint8_t Value);
        size_t AppendUInt16(uint16_t Value, bool BigEndian = true);
        size_t AppendUInt32(uint32_t Value, bool BigEndian = true);
        size_t AppendUInt64(uint64_t Value, bool BigEndian = true);
        size_t AppendInt8(int8_t Value);
        size_t AppendInt16(int16_t Value, bool BigEndian = true);
        size_t AppendInt32(int32_t Value, bool BigEndian = true);
        size_t AppendInt64(int64_t Value, bool BigEndian = true);
        size_t AppendFloat(float Value);
        size_t AppendDouble(double Value);
        size_t Append(uint8_t Value);
        size_t Append(uint16_t Value, bool BigEndian = true);
        size_t Append(uint32_t Value, bool BigEndian = true);
        size_t Append(uint64_t Value, bool BigEndian = true);
        size_t Append(int8_t Value);
        size_t Append(int16_t Value, bool BigEndian = true);
        size_t Append(int32_t Value, bool BigEndian = true);
        size_t Append(int64_t Value, bool BigEndian = true);
        size_t Append(float Value);
        size_t Append(double Value);
        size_t Append(const void * pValue, size_t Count);
        ssize_t Append(const DLMSVector& Value, size_t Position = 0, size_t Count = 0);
        size_t Append(const std::string& Value);
        size_t Append(const std::vector<uint8_t>& Value);
        size_t Append(const DLMSVariant& Value, bool Trim = true);
        void Clear();
        template <typename _VariantType>
            bool Get8(DLMSVariant * pValue)
            {
                bool RetVal = Peek8<_VariantType>(pValue);
                if (RetVal)
                {
                    m_ReadPosition += sizeof(uint8_t);
                }
                return RetVal;
            }
        template <typename _VariantType>
            bool Get16(DLMSVariant * pValue)
            {
                bool RetVal = Peek16<_VariantType>(pValue);
                if (RetVal)
                {
                    m_ReadPosition += sizeof(uint16_t);
                }
                return RetVal;
            }
        
        bool GetUInt8(DLMSVariant * pValue);
        bool GetUInt16(DLMSVariant * pValue, bool BigEndian = true);
        bool GetUInt32(DLMSVariant * pValue, bool BigEndian = true);
        bool GetUInt64(DLMSVariant * pValue, bool BigEndian = true);        
        bool GetInt8(DLMSVariant * pValue);
        bool GetInt16(DLMSVariant * pValue, bool BigEndian = true);
        bool GetInt32(DLMSVariant * pValue, bool BigEndian = true);
        bool GetInt64(DLMSVariant * pValue, bool BigEndian = true);
        bool GetFloat(DLMSVariant * pValue);
        bool GetDouble(DLMSVariant * pValue);
        bool Get(uint8_t * pValue, size_t Count);
        bool Get(std::vector<uint8_t> * pValue, size_t Count);
        std::vector<uint8_t> GetBytes() const;
        
        int Peek(size_t OffsetFromGetPosition = 0) const;
        
        template <typename _VariantType>
            bool Peek8(DLMSVariant * pValue) const
            {
                if (m_ReadPosition + sizeof(uint8_t) <= m_Data.size())
                {
                    pValue->set<_VariantType>(m_Data[m_ReadPosition]);
                    return true;
                }
                return false;
            }
        template <typename _VariantType>
            bool Peek16(DLMSVariant * pValue, bool BigEndian = true) const
            {
                static_assert(std::is_integral<_VariantType>::value &&
                              sizeof(_VariantType) >= sizeof(uint16_t),
                    "Variant type is too small");
                    
                if (m_ReadPosition + sizeof(uint16_t) <= m_Data.size())
                {
                    if (BigEndian)
                    {
                        pValue->set<_VariantType>(uint16_t(m_Data[m_ReadPosition] << 8) |
                                                  uint16_t(m_Data[m_ReadPosition + 1]));
                    }
                    else
                    {
                        pValue->set<_VariantType>(uint16_t(m_Data[m_ReadPosition + 1] << 8) |
                                                  uint16_t(m_Data[m_ReadPosition]));
                    }
                    return true;
                }
                return false;
            }
        template <typename _VariantType>
            bool Peek32(DLMSVariant * pValue, bool BigEndian = true) const
            {
                static_assert(std::is_integral<_VariantType>::value &&
                              sizeof(_VariantType) >= sizeof(uint32_t),
                    "Variant type is too small");
                
                if (m_ReadPosition + sizeof(uint32_t) <= m_Data.size())
                {
                    if (BigEndian)
                    {
                        pValue->set<_VariantType>(uint32_t(m_Data[m_ReadPosition]) << 24 |
                                  uint32_t(m_Data[m_ReadPosition + 1]) << 16 |
                                  uint32_t(m_Data[m_ReadPosition + 2]) << 8 |
                                  uint32_t(m_Data[m_ReadPosition + 3]));    
                    }
                    else
                    {
                        pValue->set<_VariantType>(uint32_t(m_Data[m_ReadPosition + 3]) << 24 |
                                  uint32_t(m_Data[m_ReadPosition + 2]) << 16 |
                                  uint32_t(m_Data[m_ReadPosition + 1]) << 8 |
                                  uint32_t(m_Data[m_ReadPosition]));
                    }
                    return true;
                }
                return false;
            }
        template <typename _VariantType>
            bool Peek64(DLMSVariant * pValue, bool BigEndian = true) const
            {
                static_assert(std::is_integral<_VariantType>::value &&
                              sizeof(_VariantType) >= sizeof(uint64_t),
                    "Variant type is too small");
                
                if (m_ReadPosition + sizeof(uint64_t) <= m_Data.size())
                {
                    if (BigEndian)
                    {
                        pValue->set<_VariantType>(uint64_t(m_Data[m_ReadPosition]) << 56 |
                                  uint64_t(m_Data[m_ReadPosition + 1]) << 48 |
                                  uint64_t(m_Data[m_ReadPosition + 2]) << 40 |
                                  uint64_t(m_Data[m_ReadPosition + 3]) << 32 |
                                  uint64_t(m_Data[m_ReadPosition + 4]) << 24 |
                                  uint64_t(m_Data[m_ReadPosition + 5]) << 16 |
                                  uint64_t(m_Data[m_ReadPosition + 6]) << 8 |
                                  uint64_t(m_Data[m_ReadPosition + 7]));    
                    }
                    else
                    {
                        pValue->set<_VariantType>(uint64_t(m_Data[m_ReadPosition + 7]) << 56 |
                                  uint64_t(m_Data[m_ReadPosition + 6]) << 48 |
                                  uint64_t(m_Data[m_ReadPosition + 5]) << 40 |
                                  uint64_t(m_Data[m_ReadPosition + 4]) << 32 |
                                  uint64_t(m_Data[m_ReadPosition + 3]) << 24 |
                                  uint64_t(m_Data[m_ReadPosition + 2]) << 16 |
                                  uint64_t(m_Data[m_ReadPosition + 1]) << 8 |
                                  uint64_t(m_Data[m_ReadPosition]));
                    }
                    return true;
                }
                return false;
            }

//        bool PeekUInt8(DLMSVariant * pValue) const;
//        bool PeekUInt16(DLMSVariant * pValue, bool BigEndian = true) const;
//        bool PeekUInt32(DLMSVariant * pValue, bool BigEndian = true) const;
//        bool PeekUInt64(DLMSVariant * pValue, bool BigEndian = true) const;
//        bool PeekInt8(DLMSVariant * pValue) const;
//        bool PeekInt16(DLMSVariant * pValue, bool BigEndian = true) const;
//        bool PeekInt32(DLMSVariant * pValue, bool BigEndian = true) const;
//        bool PeekInt64(DLMSVariant * pValue, bool BigEndian = true) const;

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
