#pragma once

#include <cstdint>
#include <cstddef>
#include <initializer_list>
#include <vector>
#include <limits>
#include <bitset>

#include "mapbox/variant.hpp"
#include "DLMSVector.h"

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
        
        enum ComponentOptions : uint32_t
        {
            NO_OPTIONS  = 0x00000000,
            EXPLICIT    = 0x01000000,
            CONSTRUCTED = 0x02000000,
            IMPLICIT    = 0x04000000,
            OPTIONAL    = 0x08000000,
        };

        enum InternalDataType : uint32_t
        {
            NORMAL_T                = 0x00000000,
            BEGIN_CHOICE_T          = 0x10000000,
            END_CHOICE_T            = 0x20000000,
            BEGIN_CHOICE_ENTRY_T    = 0x30000000,
            END_CHOICE_ENTRY_T      = 0x40000000,
            BEGIN_SEQUENCE_T        = 0x50000000,
            END_SEQUENCE_T          = 0x60000000,
            INTEGER_LIST_T          = 0x70000000,
            END_SCHEMA_T            = 0xF0000000
        };
        
        typedef uint32_t               ComponentOptionType;
        typedef uint32_t               SchemaBaseType;
        struct SchemaEntry
        {
            SchemaBaseType  m_SchemaType;
            DLMSVariant     m_Extra;
        };
        typedef const SchemaEntry *    SchemaEntryPtr;
        typedef SchemaEntryPtr         SchemaType;
        typedef uint8_t                TagIDType;
        
#define ASN_SCHEMA_INTERNAL_DATA_TYPE(SCH)\
        ((SCH)->m_SchemaType & 0xF0000000)
            
#define ASN_SCHEMA_OPTIONS(SCH)\
        ((SCH)->m_SchemaType & 0x0F000000)
            
#define ASN_SCHEMA_DATA_TYPE(SCH)\
        ASN::DataTypes((SCH)->m_SchemaType & 0x0000FFFF)
            
#define ASN_SCHEMA_DATA_TYPE_SIZE(SCH)\
        size_t(((SCH)->m_SchemaType & 0x00FF0000) >> 16)
            
#define ASN_DEFINE_SCHEMA(SDEF)\
        static const ASN::SchemaEntry SDEF[];
        
#define ASN_BEGIN_SCHEMA(SDEF) \
        const ASN::SchemaEntry SDEF[] =\
        { 
#define ASN_BEGIN_CHOICE\
            { EPRI::ASN::InternalDataType::BEGIN_CHOICE_T },
#define ASN_END_CHOICE\
            { EPRI::ASN::InternalDataType::END_CHOICE_T },
#define ASN_BEGIN_CHOICE_ENTRY(E)\
            { ((E << 16) | \
               EPRI::ASN::InternalDataType::BEGIN_CHOICE_ENTRY_T) },
#define ASN_END_CHOICE_ENTRY\
            { EPRI::ASN::InternalDataType::END_CHOICE_ENTRY_T },
#define ASN_BEGIN_SEQUENCE(OPTIONS)\
            { (EPRI::ASN::InternalDataType::BEGIN_SEQUENCE_T | (OPTIONS)) },
#define ASN_END_SEQUENCE\
            { EPRI::ASN::InternalDataType::END_SEQUENCE_T },
#define ASN_NULL_TYPE\
            { EPRI::ASN::DataTypes::NULL },
#define ASN_OCTET_STRING_TYPE(OPTIONS)\
            { (EPRI::ASN::DataTypes::OCTET_STRING | (OPTIONS)) },
#define ASN_BIT_STRING_TYPE(OPTIONS, MAXSIZE)\
            { ((OPTIONS) | (EPRI::ASN::SchemaBaseType(MAXSIZE) << 16) | \
                EPRI::ASN::DataTypes::BIT_STRING) },
#define ASN_OBJECT_IDENTIFIER_TYPE(OPTIONS)\
            { (EPRI::ASN::DataTypes::OBJECT_IDENTIFIER | (OPTIONS)) },
#define ASN_INTEGER_TYPE(OPTIONS)\
            { (EPRI::ASN::DataTypes::INTEGER | (OPTIONS)) },
#define ASN_INTEGER_LIST_TYPE(OPTIONS, ...)\
            { (EPRI::ASN::InternalDataType::INTEGER_LIST_T | (OPTIONS)), \
                    DLMSVariant(std::initializer_list<uint32_t>(__VA_ARGS__)) },
#define ASN_BOOLEAN_TYPE(OPTIONS)\
            { (EPRI::ASN::DataTypes::BOOLEAN | (OPTIONS)) },
#define ASN_ENUM_TYPE(OPTIONS)\
            { (EPRI::ASN::DataTypes::ENUM | (OPTIONS)) },
#define ASN_REAL_TYPE(OPTIONS)\
            { (EPRI::ASN::DataTypes::REAL | (OPTIONS)) },
#define ASN_GraphicString_TYPE(OPTIONS)\
            { (EPRI::ASN::DataTypes::GraphicString | (OPTIONS)) },
#define ASN_END_SCHEMA\
            { EPRI::ASN::InternalDataType::END_SCHEMA_T }\
        };
        //
        // Standard Schema Definitions
        //
        extern const SchemaEntry EmptySchema[];
        extern const SchemaEntry OctetStringSchema[];
        extern const SchemaEntry ObjectIdentifierSchema[];
        extern const SchemaEntry IntegerSchema[];
        extern const SchemaEntry GraphicStringSchema[];

    }
    
    class ASNType
    {
    public:
        ASNType() = delete;
        ASNType(ASN::SchemaEntryPtr Schema);
        ASNType(ASN::DataTypes DT, const DLMSVariant& Value);
        virtual ~ASNType();
        
        virtual std::vector<uint8_t> GetBytes() const;
        virtual std::vector<uint8_t> GetBytes(ASN::TagIDType Tag, ASN::ComponentOptionType Options);

        virtual bool IsEmpty() const;
        virtual void Clear();
        virtual void Rewind();
        virtual bool Append(const DLMSVariant& Value);
        virtual bool Append(const ASNType& Value);
        virtual bool GetCurrentSchemaValue(DLMSVariant * pVariant) const;
        
        inline ASN::DataTypes GetCurrentSchemaType() const
        {
            return ASN_SCHEMA_DATA_TYPE(GetCurrentSchemaEntry());
        }
        inline uint32_t GetCurrentSchemaTypeSize() const
        {
            return ASN_SCHEMA_DATA_TYPE_SIZE(GetCurrentSchemaEntry());
        }
        //
        // Operators
        //
        bool operator==(const std::vector<uint8_t>& rhs);
        //
        // Helpers
        //
        static bool AppendLength(size_t Length, DLMSVector * pData);
        static uint8_t CalculateLengthBytes(size_t Length);

    protected:
        ASNType(ASN::DataTypes DT);
        
        inline ASN::SchemaEntryPtr GetCurrentSchemaEntry() const
        {
            return m_pCurrentSchema;
        }
        
        bool GetNextSchemaEntry(ASN::SchemaEntryPtr * ppSchemaEntry);
        bool InternalAppend(const DLMSVariant& Value);
        bool InternalAppend(const ASNType& Value);
        bool InternalAppend(const DLMSVector& Value);
       
        enum AppendStates
        {
            ST_SIMPLE,
            ST_CHOICE,
            ST_SEQUENCE
        }                              m_AppendState = ST_SIMPLE;
        ASN::SchemaEntry               m_SingleDataType[2] = { { ASN::VOID }, { ASN::END_SCHEMA_T } };
        ASN::SchemaEntryPtr            m_pSchema = nullptr;
        ASN::SchemaEntryPtr            m_pCurrentSchema = nullptr;
        DLMSVector                     m_Data;
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