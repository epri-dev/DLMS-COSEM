#pragma once

#include <cstdint>
#include <cstddef>
#include <initializer_list>
#include <vector>
#include <limits>
#include <bitset>
#include <limits>
#include <stack>

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

            DT_Base           = 200,
            DT_Integer8       = DT_Base + DLMSVariantIndex::VAR_INT8,
            DT_Unsigned8      = DT_Base + DLMSVariantIndex::VAR_UINT8,
            DT_Integer16      = DT_Base + DLMSVariantIndex::VAR_INT16,
            DT_Unsigned16     = DT_Base + DLMSVariantIndex::VAR_UINT16,
            DT_Integer32      = DT_Base + DLMSVariantIndex::VAR_INT32,
            DT_Unsigned32     = DT_Base + DLMSVariantIndex::VAR_UINT32,
            DT_Integer64      = DT_Base + DLMSVariantIndex::VAR_INT64,
            DT_Unsigned64     = DT_Base + DLMSVariantIndex::VAR_UINT64,
            
            DT_Data           = DT_Base + 50,
            
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

#define ASN_IS_SCHEMA_END(SCH)\
        (EPRI::ASN::InternalDataType::END_SCHEMA_T == ASN_SCHEMA_INTERNAL_DATA_TYPE(SCH))
            
#define ASN_SCHEMA_OPTIONS(SCH)\
        ASN::ComponentOptionType((SCH)->m_SchemaType & 0x0F000000)

#define ASN_IS_IMPLICIT(SCH)\
        (ASN_SCHEMA_OPTIONS(SCH) & ASN::IMPLICIT)

#define ASN_IS_CONSTRUCTED(SCH)\
        (ASN_SCHEMA_OPTIONS(SCH) & ASN::CONSTRUCTED)

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
#define ASN_BEGIN_CHOICE_ENTRY_WITH_OPTIONS(E, OPTIONS)\
            { ((E << 16) | \
               EPRI::ASN::InternalDataType::BEGIN_CHOICE_ENTRY_T |\
               (OPTIONS)) },
#define ASN_END_CHOICE_ENTRY\
            { EPRI::ASN::InternalDataType::END_CHOICE_ENTRY_T },
#define ASN_BEGIN_SEQUENCE\
            { (EPRI::ASN::InternalDataType::BEGIN_SEQUENCE_T) },
#define ASN_BEGIN_SEQUENCE_WITH_OPTIONS(OPTIONS)\
            { (EPRI::ASN::InternalDataType::BEGIN_SEQUENCE_T | (OPTIONS)) },
#define ASN_END_SEQUENCE\
            { EPRI::ASN::InternalDataType::END_SEQUENCE_T },
#define ASN_NULL_TYPE\
            { EPRI::ASN::DataTypes::NULL },
#define ASN_OCTET_STRING_TYPE(OPTIONS)\
            { (EPRI::ASN::DataTypes::OCTET_STRING | (OPTIONS)) },
#define ASN_FIXED_OCTET_STRING_TYPE(OPTIONS, SIZE)\
            { ((OPTIONS) | (EPRI::ASN::SchemaBaseType(SIZE) << 16) | \
                EPRI::ASN::DataTypes::OCTET_STRING) },
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
#define ASN_FIXED_INTEGER_LIST_TYPE(OPTIONS, INTSIZE, ...)\
            { (EPRI::ASN::InternalDataType::INTEGER_LIST_T | (OPTIONS) | (EPRI::ASN::SchemaBaseType(INTSIZE) << 16)), \
                    DLMSVariant(std::initializer_list<uint32_t>(__VA_ARGS__)) },
#define ASN_BOOLEAN_TYPE(OPTIONS)\
            { (EPRI::ASN::DataTypes::BOOLEAN | (OPTIONS)) },
#define ASN_ENUM_TYPE\
            { (EPRI::ASN::DataTypes::ENUM) },
#define ASN_ENUM_TYPE_WITH_OPTIONS(OPTIONS)\
            { (EPRI::ASN::DataTypes::ENUM | (OPTIONS)) },
#define ASN_REAL_TYPE(OPTIONS)\
            { (EPRI::ASN::DataTypes::REAL | (OPTIONS)) },
#define ASN_GraphicString_TYPE(OPTIONS)\
            { (EPRI::ASN::DataTypes::GraphicString | (OPTIONS)) },
#define ASN_BASE_TYPE(DT)\
            { DT },
#define ASN_DATA_TYPE\
            { EPRI::ASN::DataTypes::DT_Data },
#define ASN_END_SCHEMA\
            { EPRI::ASN::InternalDataType::END_SCHEMA_T }\
        };
        
#define ASN_MAKE_TAG(TAG, OPTIONS)\
        (TAG | (OPTIONS & ASN::CONSTRUCTED ? 0b00100000 : 0x00))        
        //
        // Standard Schema Definitions
        //
        extern const SchemaEntry EmptySchema[];
        extern const SchemaEntry OctetStringSchema[];
        extern const SchemaEntry ObjectIdentifierSchema[];
        extern const SchemaEntry IntegerSchema[];
        extern const SchemaEntry GraphicStringSchema[];
        extern const SchemaEntry ImplicitOctetStringSchema[];
        extern const SchemaEntry ImplicitObjectIdentifierSchema[];

    }
    
    class ASNObjectIdentifier;
    class ASNBitString;
   
    class ASNType
    {
        friend class ASNObjectIdentifier;
        friend class ASNBitString;
        
    public:
        ASNType();
        ASNType(ASN::SchemaEntryPtr Schema);
        ASNType(ASN::SchemaBaseType DT, const DLMSVariant& Value);
        virtual ~ASNType();
        
        virtual std::vector<uint8_t> GetBytes() const;
        virtual std::vector<uint8_t> GetBytes(ASN::TagIDType Tag, ASN::ComponentOptionType Options);

        virtual bool Parse(DLMSVector * pData);

        virtual bool IsEmpty() const;
        virtual void Clear();
        virtual void Rewind();
        virtual bool SelectChoice(int8_t Choice);
        virtual bool GetChoice(int8_t * pChoice);
        virtual bool Append(const DLMSValue& Value);
        
        enum GetNextResult
        {
            VALUE_RETRIEVED = 0,
            END_OF_SCHEMA,
            NO_VALUE_FOUND,
            SCHEMA_MISMATCH,
            INVALID_STREAM,
            INVALID_CONDITION,
            VALUE_EMPTY
        };
        
        virtual GetNextResult GetNextValue(ASNType * pValue);
        virtual GetNextResult GetNextValue(DLMSValue * pValue);
        
        inline ASN::DataTypes GetCurrentSchemaType() const
        {
            return ASN_SCHEMA_DATA_TYPE(GetCurrentSchemaEntry());
        }
        inline ASN::ComponentOptionType GetCurrentSchemaOptions() const
        {
            return ASN_SCHEMA_OPTIONS(GetCurrentSchemaEntry());
        }
        inline uint32_t GetCurrentSchemaTypeSize() const
        {
            return ASN_SCHEMA_DATA_TYPE_SIZE(GetCurrentSchemaEntry());
        }
        inline bool IsCurrentSchemaImplicit() const
        {
            return ASN_IS_IMPLICIT(GetCurrentSchemaEntry());
        }
        //
        // Operators
        //
        bool operator==(const std::vector<uint8_t>& rhs) const;
        bool operator==(const ASNType& rhs) const;
        //
        // Helpers
        //
        static bool AppendLength(size_t Length, DLMSVector * pData);
        static bool GetLength(DLMSVector * pData, size_t * pLength);
        static bool PeekLength(const DLMSVector& Data, size_t Offset, size_t * pLength, size_t * pBytes = nullptr);
        static uint8_t CalculateLengthBytes(size_t Length);

    protected:
        ASNType(ASN::SchemaBaseType DT);
        void SetSchemaType(ASN::SchemaBaseType DT);
        
        inline ASN::SchemaEntryPtr GetCurrentSchemaEntry() const
        {
            return m_pCurrentSchema;
        }
        
        GetNextResult GetNextSchemaEntry(ASN::SchemaEntryPtr * ppSchemaEntry);
        
        bool InternalAppend(const DLMSValue& Value);
        bool InternalAppend(ASNType * pValue);
        bool InternalSimpleAppend(ASN::SchemaEntryPtr SchemaEntry, const DLMSVariant& Value);
        bool InternalAppend(const DLMSVector& Value);

        GetNextResult InternalSimpleGet(ASN::SchemaEntryPtr SchemaEntry, DLMSVariant * pValue);
        GetNextResult GetINTEGER(ASN::SchemaEntryPtr SchemaEntry, DLMSVariant * pValue);
        GetNextResult GetSTRING(ASN::SchemaEntryPtr SchemaEntry, DLMSVariant * pValue);
     
        const int8_t                  INVALID_CHOICE = std::numeric_limits<int8_t>::lowest();
        enum ParseStates
        {
            ST_SIMPLE,
            ST_CHOICE,
            ST_CHOICE_ENTRY,
            ST_SEQUENCE
        };
        struct ParseState
        {
            ParseState(ASN::SchemaEntryPtr SchemaEntry,
                ParseStates State,
                int8_t Choice)
                : m_SchemaEntry(SchemaEntry)
                , m_State(State)
                , m_Choice(Choice)
            {
            }
            ASN::SchemaEntryPtr        m_SchemaEntry;
            ParseStates                m_State;
            int8_t                     m_Choice;
        };
        std::stack<ParseState>         m_GetStates;
        std::stack<ParseState>         m_AppendStates;
        //
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
        ASNObjectIdentifier(ArcList List, ASN::ComponentOptions Options = ASN::NO_OPTIONS, OIDType OT = ABSOLUTE);
        virtual ~ASNObjectIdentifier();

        virtual bool Get(ArcVector * pVector);
        //
        // Operators
        //
        operator DLMSVariant() const;
        //
        // Helpers
        //
        static bool Peek(ASN::SchemaEntryPtr SchemaEntry, const ASNType& Value, DLMSVariant * pVariant, size_t * pBytes = nullptr);
        static bool Get(ASN::SchemaEntryPtr SchemaEntry, ASNType * pValue, DLMSVariant * pVariant);
        
    };
    
    class ASNBitString : public ASNType
    {
    public:
        ASNBitString();
        virtual ~ASNBitString();
        ASNBitString(size_t BitsExpected, const DLMSBitSet& Value);
        ASNBitString(size_t BitsExpected);
         //
        // Operators
        //
        operator DLMSVariant() const;
        //
        // Helpers
        //
        static bool Peek(ASN::SchemaEntryPtr SchemaEntry, const ASNType& Value, DLMSVariant * pVariant, size_t * pBytes = nullptr);
        static bool Get(ASN::SchemaEntryPtr SchemaEntry, ASNType * pValue, DLMSVariant * pVariant);
        
    protected:
        size_t m_BitsExpected;
        
    };
    
    class ASNVoid : public ASNType
    {
    public:
        ASNVoid();
    };
    
}