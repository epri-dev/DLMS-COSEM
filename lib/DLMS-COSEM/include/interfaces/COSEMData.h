#pragma once

#include <cstdint>
#include <vector>
#include <stack>
#include <limits>

#include "DLMSVector.h"

namespace EPRI
{
    enum COSEMDataType : uint8_t
    {
        NULL_DATA            = 0,
        ARRAY                = 1,
        STRUCTURE            = 2,
        BOOLEAN              = 3,
        BIT_STRING           = 4,
        DOUBLE_LONG          = 5,
        DOUBLE_LONG_UNSIGNED = 6,
        FLOATING_POINT       = 7,
        OCTET_STRING         = 9,
        VISIBLE_STRING       = 10,
        UTF8_STRING          = 12,
        BCD                  = 13,
        INTEGER              = 15,
        LONG                 = 16,
        UNSIGNED             = 17,
        LONG_UNSIGNED        = 18,
        COMPACT_ARRAY        = 19,
        LONG64               = 20,
        LONG64_UNSIGNED      = 21,
        ENUM                 = 22,
        FLOAT32              = 23,
        FLOAT64              = 24,
        DATE_TIME            = 25,
        DATE                 = 26,
        TIME                 = 27,
        DONT_CARE            = 255
    };
    
    typedef int32_t DOUBLE_LONG_CType;
    typedef uint32_t DOUBLE_LONG_UNSIGNED_CType;
    typedef DLMSVector OCTET_STRING_CType;
    typedef std::string VISIBLE_STRING_CType;
    typedef int8_t INTEGER_CType;
    typedef int16_t LONG_CType;
    typedef uint8_t UNSIGNED_CType;
    typedef uint16_t LONG_UNSIGNED_CType;
    typedef int64_t LONG64_CType;
    typedef uint64_t LONG64_UNSIGNED_CType;
    typedef DLMSBitSet BIT_STRING_CType;

    enum COSEMSchemaOptions : uint32_t
    {
        NO_OPTIONS  = 0x00000000
    };
    
    enum COSEMInternalDataType : uint32_t
    {
        NORMAL_T              = 0x00000000,
        BEGIN_SPECIAL_T       = 0x10000000,
        END_SPECIAL_T         = 0x20000000,
        BEGIN_SPECIAL_ENTRY_T = 0x30000000,
        END_SPECIAL_ENTRY_T   = 0x40000000,
        BEGIN_CHOICE_T        = 0x50000000,
        END_CHOICE_T          = 0x60000000,
        END_SCHEMA_T          = 0xF0000000
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
    
#define COSEM_SCHEMA_INTERNAL_DATA_TYPE(SCH)\
        ((SCH)->m_SchemaType & 0xF0000000)

#define COSEM_IS_SCHEMA_END(SCH)\
        (EPRI::COSEMInternalDataType::END_SCHEMA_T == COSEM_SCHEMA_INTERNAL_DATA_TYPE(SCH))

#define COSEM_IS_ARRAY_BEGIN(SCH)\
        (((EPRI::COSEMInternalDataType::BEGIN_SPECIAL_T | EPRI::COSEMDataType::ARRAY) & \
         (SCH)->m_SchemaType) == (EPRI::COSEMInternalDataType::BEGIN_SPECIAL_T | EPRI::COSEMDataType::ARRAY))
        
#define COSEM_IS_ARRAY_END(SCH)\
        (((EPRI::COSEMInternalDataType::END_SPECIAL_T | EPRI::COSEMDataType::ARRAY) & \
         (SCH)->m_SchemaType) == (EPRI::COSEMInternalDataType::END_SPECIAL_T | EPRI::COSEMDataType::ARRAY))

#define COSEM_IS_STRUCTURE_BEGIN(SCH)\
        (((EPRI::COSEMInternalDataType::BEGIN_SPECIAL_T | EPRI::COSEMDataType::STRUCTURE) & \
         (SCH)->m_SchemaType) == (EPRI::COSEMInternalDataType::BEGIN_SPECIAL_T | EPRI::COSEMDataType::STRUCTURE))
        
#define COSEM_IS_STRUCTURE_END(SCH)\
        (((EPRI::COSEMInternalDataType::END_SPECIAL_T | EPRI::COSEMDataType::STRUCTURE) & \
         (SCH)->m_SchemaType) == (EPRI::COSEMInternalDataType::END_SPECIAL_T | EPRI::COSEMDataType::STRUCTURE))
        
#define COSEM_SCHEMA_OPTIONS(SCH)\
        COSEMSchemaOptions((SCH)->m_SchemaType & 0x0F000000)

#define COSEM_SCHEMA_DATA_TYPE(SCH)\
        COSEMDataType((SCH)->m_SchemaType & 0x000000FF)
            
#define COSEM_SCHEMA_DATA_TYPE_SIZE(SCH)\
        size_t(((SCH)->m_SchemaType & 0x00FF0000) >> 16)

#define COSEM_DEFINE_SCHEMA(SDEF)\
        static const SchemaEntry SDEF[];
    
#define COSEM_BEGIN_SCHEMA(SDEF) \
        const SchemaEntry SDEF[] =\
        { 
#define COSEM_BEGIN_ARRAY\
            { (EPRI::COSEMInternalDataType::BEGIN_SPECIAL_T | COSEMDataType::ARRAY) },
#define COSEM_END_ARRAY\
            { (EPRI::COSEMInternalDataType::END_SPECIAL_T | COSEMDataType::ARRAY) },
#define COSEM_BEGIN_STRUCTURE\
            { (EPRI::COSEMInternalDataType::BEGIN_SPECIAL_T | COSEMDataType::STRUCTURE) },
#define COSEM_END_STRUCTURE\
            { (EPRI::COSEMInternalDataType::END_SPECIAL_T | COSEMDataType::STRUCTURE) },
#define COSEM_BEGIN_COMPACT_ARRAY\
            { (EPRI::COSEMInternalDataType::BEGIN_SPECIAL_T | COSEMDataType::COMPACT_ARRAY) },
#define COSEM_END_COMPACT_ARRAY\
            { (EPRI::COSEMInternalDataType::END_SPECIAL_T | COSEMDataType::COMPACT_ARRAY) },
#define COSEM_BEGIN_CHOICE\
            { (EPRI::COSEMInternalDataType::BEGIN_CHOICE_T) },
#define COSEM_END_CHOICE\
            { (EPRI::COSEMInternalDataType::END_CHOICE_T) },
#define COSEM_NULL_DATA_TYPE\
            { EPRI::COSEMDataType::NULL_DATA },
#define COSEM_BOOLEAN_TYPE\
            { EPRI::COSEMDataType::BOOLEAN },
#define COSEM_BIT_STRING_TYPE\
            { EPRI::COSEMDataType::BIT_STRING },
#define COSEM_DOUBLE_LONG_TYPE\
            { EPRI::COSEMDataType::DOUBLE_LONG },
#define COSEM_DOUBLE_LONG_UNSIGNED_TYPE\
            { EPRI::COSEMDataType::DOUBLE_LONG_UNSIGNED },
#define COSEM_FLOATING_POINT_TYPE\
            { EPRI::COSEMDataType::FLOATING_POINT },
#define COSEM_OCTET_STRING_TYPE\
            { EPRI::COSEMDataType::OCTET_STRING },
#define COSEM_VISIBLE_STRING_TYPE\
            { EPRI::COSEMDataType::VISIBLE_STRING },
#define COSEM_UTF8_STRING_TYPE\
            { EPRI::COSEMDataType::UTF8_STRING },
#define COSEM_BCD_TYPE\
            { EPRI::COSEMDataType::BCD_TYPE },
#define COSEM_INTEGER_TYPE\
            { EPRI::COSEMDataType::INTEGER },
#define COSEM_LONG_TYPE\
            { EPRI::COSEMDataType::LONG },
#define COSEM_UNSIGNED_TYPE\
            { EPRI::COSEMDataType::UNSIGNED },
#define COSEM_LONG_UNSIGNED_TYPE\
            { EPRI::COSEMDataType::LONG_UNSIGNED },
#define COSEM_LONG64_TYPE\
            { EPRI::COSEMDataType::LONG64 },
#define COSEM_LONG64_UNSIGNED_TYPE\
            { EPRI::COSEMDataType::LONG64_UNSIGNED },
#define COSEM_ENUM_TYPE(...)\
            { EPRI::COSEMDataType::ENUM, \
              DLMSVariant(std::initializer_list<uint32_t>(__VA_ARGS__)) },
#define COSEM_FLOAT32_TYPE\
            { EPRI::COSEMDataType::FLOAT32 },
#define COSEM_FLOAT64_TYPE\
            { EPRI::COSEMDataType::FLOAT64 },
#define COSEM_DATE_TIME_TYPE\
            { EPRI::COSEMDataType::DATE_TIME },
#define COSEM_DATE_TYPE\
            { EPRI::COSEMDataType::DATE },
#define COSEM_TIME_TYPE\
            { EPRI::COSEMDataType::TIME },
#define COSEM_END_SCHEMA\
        };
    //
    // Standard Schema Definitions
    //
    extern const SchemaEntry OctetStringSchema[];
    extern const SchemaEntry LongSchema[];
    extern const SchemaEntry UnsignedSchema[];
    extern const SchemaEntry IntegerSchema[];
    extern const SchemaEntry BooleanSchema[];
    
    class COSEMBitString;
    
    class COSEMType
    {
        friend class COSEMBitString;
        
    public:
        COSEMType();
        COSEMType(SchemaEntryPtr Schema);
        COSEMType(SchemaBaseType DT, const DLMSVariant& Value);
        virtual ~COSEMType();
        
        virtual void GetBytes(DLMSVector * pDestination) const;
        virtual bool Parse(DLMSVector * pData);

        virtual bool IsEmpty() const;
        virtual void Clear();
        virtual void Rewind();
        virtual bool SelectChoice(COSEMDataType Choice);
        virtual bool GetChoice(COSEMDataType * pChoice);
        virtual bool Append(const DLMSValue& Value);
        virtual bool Append();
       
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
        
        virtual GetNextResult GetNextValue(DLMSValue * pValue);
        
        inline COSEMDataType GetCurrentSchemaType() const
        {
            return COSEM_SCHEMA_DATA_TYPE(GetCurrentSchemaEntry());
        }
        inline COSEMSchemaOptions GetCurrentSchemaOptions() const
        {
            return COSEM_SCHEMA_OPTIONS(GetCurrentSchemaEntry());
        }
        inline size_t GetCurrentSchemaTypeSize() const
        {
            return COSEM_SCHEMA_DATA_TYPE_SIZE(GetCurrentSchemaEntry());
        }
        //
        // Operators
        //
        bool operator==(const std::vector<uint8_t>& rhs) const;
        bool operator==(const COSEMType& rhs) const;
        COSEMType& operator=(const DLMSVector& rhs);

    protected:
        COSEMType(SchemaBaseType DT);
        void SetSchemaType(SchemaBaseType DT);
        
        inline SchemaEntryPtr GetCurrentSchemaEntry() const
        {
            return m_pCurrentSchema;
        }
        
        GetNextResult GetNextSchemaEntry(SchemaEntryPtr * ppSchemaEntry);
        
        bool InternalAppend(const DLMSValue& Value);
        bool InternalAppend(COSEMType * pValue);
        bool InternalSimpleAppend(SchemaEntryPtr SchemaEntry, const DLMSVariant& Value);
        bool InternalAppend(const DLMSVector& Value);

        GetNextResult InternalSimpleGet(SchemaEntryPtr SchemaEntry, DLMSVariant * pValue);
     
        const COSEMDataType      INVALID_CHOICE = COSEMDataType::DONT_CARE;
        enum ParseStates
        {
            ST_SIMPLE,
            ST_CHOICE,
            ST_STRUCTURE,
            ST_ARRAY
        };
        struct ParseState
        {
            ParseState(SchemaEntryPtr SchemaEntry,
                ParseStates State,
                COSEMDataType Choice)
                : m_SchemaEntry(SchemaEntry)
                , m_State(State)
                , m_Choice(Choice)
            {
            }
            SchemaEntryPtr       m_SchemaEntry;
            ParseStates          m_State;
            COSEMDataType        m_Choice;
        };
        std::stack<ParseState>   m_GetStates;
        std::stack<ParseState>   m_AppendStates;
        //
        SchemaEntry               m_SingleDataType[2] = { { NULL_DATA }, { END_SCHEMA_T } };
        SchemaEntryPtr            m_pSchema = nullptr;
        SchemaEntryPtr            m_pCurrentSchema = nullptr;
        DLMSVector                m_Data;
    };

    class COSEMBitString : public COSEMType
    {
    public:
        COSEMBitString();
        virtual ~COSEMBitString();
        COSEMBitString(size_t BitsExpected, const DLMSBitSet& Value);
        COSEMBitString(size_t BitsExpected);
        size_t GetBitLength() const;
        //
        // Operators
        //
        operator DLMSVariant() const;
        //
        // Helpers
        //
        static bool Peek(SchemaEntryPtr SchemaEntry, const COSEMType& Value, DLMSVariant * pVariant, size_t * pBytes = nullptr);
        static bool Get(SchemaEntryPtr SchemaEntry, COSEMType * pValue, DLMSVariant * pVariant);
        
    protected:
        size_t m_BitsExpected;
        
    };

}
