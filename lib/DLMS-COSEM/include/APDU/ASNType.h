#pragma once

#include <cstdint>
#include <cstddef>
#include <initializer_list>
#include <vector>
#include <limits>
#include <bitset>

#include "mapbox/variant.hpp"

namespace EPRI
{
    namespace ASN
    {
        enum DataTypes : uint8_t
        {
            BOOLEAN           = 1,
            INTEGER           = 2,
            BIT_STRING        = 3,
            OCTET_STRING      = 4,
            NULL_TYPE         = 5,
            OBJECT_IDENTIFIER = 6,
            REAL              = 9,
            ENUM              = 10,
            NumericString     = 18,
            PrintableString   = 19,
            TeletexString     = 20,
            VisibleString     = 26,
            GraphicString     = 25,
            VOID              = 0xFF
        };
        
        enum ComponentOptions : uint8_t
        {
            EXPLICIT = 0b00100000,
            DEFAULT  = 0b00000001,
            IMPLICIT = 0b00000010,
            OPTIONAL = 0b00000100,
        };

        enum InternalDataType : uint32_t
        {
            NORMAL_T       = 0x00000000,
            BEGIN_CHOICE_T = 0x10000000,
            END_CHOICE_T   = 0x20000000,
            END_SCHEMA_T   = 0xFFFF0000
        };
        
        typedef uint8_t                ComponentOptionType;
        typedef uint32_t               SchemaBaseType;
        typedef const SchemaBaseType * SchemaType;
        typedef uint8_t                TagIDType;
        
#define ASN_SCHEMA_INTERNAL_DATA_TYPE(SCH)\
        (SCH & 0xF0000000)
#define ASN_SCHEMA_DATA_TYPE(SCH)\
        ASN::DataTypes(SCH & 0x0000FFFF)
#define ASN_SCHEMA_DATA_TYPE_MAX_LENGTH(SCH)\
        size_t((SCH & 0x0FFF0000) >> 16)
            
#define ASN_DEFINE_SCHEMA(SDEF)\
        static const ASN::SchemaBaseType SDEF[];
        
#define ASN_BEGIN_SCHEMA(SDEF) \
        const ASN::SchemaBaseType SDEF[] =\
        { 
#define ASN_BEGIN_CHOICE\
            EPRI::ASN::InternalDataType::BEGIN_CHOICE_T,
#define ASN_END_CHOICE\
            EPRI::ASN::InternalDataType::END_CHOICE_T,
#define ASN_NULL_TYPE\
            EPRI::ASN::DataTypes::NULL,
#define ASN_OCTET_STRING_TYPE\
            EPRI::ASN::DataTypes::OCTET_STRING,
#define ASN_BIT_STRING_TYPE_AND_SIZE(MAXSIZE)\
            EPRI::ASN::InternalDataType::NORMAL_T | (EPRI::ASN::SchemaBaseType(MAXSIZE) << 16) | \
                EPRI::ASN::DataTypes::BIT_STRING,
#define ASN_OBJECT_IDENTIFIER_TYPE\
            EPRI::ASN::DataTypes::OBJECT_IDENTIFIER,
#define ASN_INTEGER_TYPE\
            EPRI::ASN::DataTypes::INTEGER,
#define ASN_BOOLEAN_TYPE\
            EPRI::ASN::DataTypes::BOOLEAN,
#define ASN_ENUM_TYPE\
            EPRI::ASN::DataTypes::ENUM,
#define ASN_REAL_TYPE\
            EPRI::ASN::DataTypes::REAL,
#define ASN_GraphicString_TYPE\
            EPRI::ASN::DataTypes::GraphicString,
#define ASN_END_SCHEMA\
            EPRI::ASN::InternalDataType::END_SCHEMA_T\
        };
        //
        // Standard Schema Definitions
        //
        extern const SchemaBaseType EmptySchema[];
        extern const SchemaBaseType OctetStringSchema[];
        extern const SchemaBaseType ObjectIdentifierSchema[];
        extern const SchemaBaseType IntegerSchema[];
        extern const SchemaBaseType GraphicStringSchema[];

    }
    
    using namespace mapbox::util;
    
    struct blank
        {};
    
    using ASNVariant = variant<blank, bool, int32_t, uint32_t, std::string, double, std::vector<uint8_t>, std::initializer_list<uint32_t>>;
    enum ASNVariantIndex
    {
        VAR_BLANK     = 0,
        VAR_BOOL      = 1,
        VAR_INT32     = 2,
        VAR_UINT32    = 3,
        VAR_STRING    = 4,
        VAR_DOUBLE    = 5,
        VAR_VECTOR    = 6,
        VAR_INIT_LIST = 7
    };

    class ASNType
    {
    public:
        ASNType() = delete;
        ASNType(ASN::SchemaType Schema);
        ASNType(ASN::DataTypes DT, const ASNVariant& Value);
        virtual ~ASNType();
        
        virtual std::vector<uint8_t> GetBytes() const;
        virtual std::vector<uint8_t> GetBytes(ASN::TagIDType Tag, ASN::ComponentOptionType Options);

        virtual bool IsEmpty() const;
        virtual void Clear();
        virtual void Rewind();
        virtual bool Append(const ASNVariant& Value);
        virtual bool Append(const ASNType& Value);
        virtual bool Get(ASNVariant * pValue);
        
        inline ASN::DataTypes GetCurrentSchemaType() const
        {
            return ASN_SCHEMA_DATA_TYPE(GetCurrentSchemaEntry());
        }
        inline size_t GetCurrentSchemaTypeMaxLength() const
        {
            return ASN_SCHEMA_DATA_TYPE_MAX_LENGTH(GetCurrentSchemaEntry());
        }
        //
        // Operators
        //
        bool operator==(const std::vector<uint8_t>& rhs);
        //
        // Helpers
        //
        static bool AppendLength(size_t Length, std::vector<uint8_t> * pData);
        static uint8_t CalculateLengthBytes(size_t Length);

    protected:
        using ASNRawDataType = std::vector<uint8_t>;

        ASNType(ASN::DataTypes DT);
        
        inline ASN::SchemaBaseType GetCurrentSchemaEntry() const
        {
            return *m_pCurrentSchema;
        }
        
        bool GetNextSchemaEntry(ASN::SchemaBaseType * pSchemaEntry);
        bool InternalAppend(const ASNVariant& Value);
        bool InternalAppend(const ASNType& Value);
        bool InternalAppend(const ASNRawDataType& Value);
       
        enum AppendStates
        {
            SIMPLE,
            CHOICE
        }                       m_AppendState = SIMPLE;
        ASN::SchemaBaseType     m_SingleDataType[2] = { ASN::VOID, ASN::END_SCHEMA_T};
        ASN::SchemaType         m_pSchema = nullptr;
        ASN::SchemaType         m_pCurrentSchema = nullptr;
        ASNRawDataType          m_Data;
    };
    
    
    
    class ASNObjectIdentifier : public ASNType
    {
        using ArcList = std::initializer_list<uintmax_t>;
        using ArcVector = std::vector<uintmax_t>;
        
    public:
        enum OIDType : uint8_t
        {
            ABSOLUTE = 0x06,
            RELATIVE = 0x80
        };
        
        ASNObjectIdentifier() = delete;
        ASNObjectIdentifier(ArcList List, OIDType OT = ABSOLUTE);

        virtual ~ASNObjectIdentifier();

        virtual bool Get(ArcVector * pVector);
        
    };
    
    class ASNBitString : public ASNType
    {
    public:
        using ASNBitSet = std::bitset<64>;
        
        ASNBitString();
        virtual ~ASNBitString();
        ASNBitString(size_t BitsExpected, const ASNBitSet& Value);
        
        virtual bool Get(ASNBitSet * pBitSet);
        
    protected:
        size_t m_BitsExpected;
        
    };
    
    class ASNVoid : public ASNType
    {
    public:
        ASNVoid();
    };

    
}