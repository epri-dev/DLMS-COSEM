#include <algorithm>
#include <set>
#include <stdexcept>

#include "APDU/ASNType.h"
#include "interfaces/COSEMData.h"

namespace EPRI
{
    COSEM_BEGIN_SCHEMA(OctetStringSchema)
        COSEM_OCTET_STRING_TYPE
    COSEM_END_SCHEMA
    COSEM_BEGIN_SCHEMA(LongSchema)
        COSEM_LONG_TYPE
    COSEM_END_SCHEMA
    COSEM_BEGIN_SCHEMA(UnsignedSchema)
        COSEM_UNSIGNED_TYPE
    COSEM_END_SCHEMA
    COSEM_BEGIN_SCHEMA(IntegerSchema)
        COSEM_INTEGER_TYPE
    COSEM_END_SCHEMA
    COSEM_BEGIN_SCHEMA(BooleanSchema)
        COSEM_BOOLEAN_TYPE
    COSEM_END_SCHEMA
        
    //
    // COSEMType
    //
    COSEMType::COSEMType()
    {
    }

    COSEMType::COSEMType(SchemaEntryPtr Schema)
        : m_pSchema(Schema)
        , m_pCurrentSchema(Schema)
    {
    }
    
    COSEMType::COSEMType(SchemaBaseType DT)
        : m_pSchema(m_SingleDataType)
        , m_pCurrentSchema(m_SingleDataType)
    {
        m_SingleDataType[0].m_SchemaType = DT;
    }
    
    COSEMType::COSEMType(SchemaBaseType DT, const DLMSVariant& Value)
        : m_pSchema(m_SingleDataType)
        , m_pCurrentSchema(m_SingleDataType)
    {
        m_SingleDataType[0].m_SchemaType = DT;
        InternalAppend(Value);
        Rewind();
    }

    COSEMType::~COSEMType()
    {
    }
    
    void COSEMType::GetBytes(DLMSVector * pDestination) const
    {
        pDestination->Append(m_Data.GetBytes());
    }
        
    bool COSEMType::IsEmpty() const
    {
        return m_Data.Size() == 0;
    }

    void COSEMType::Clear()
    {
        m_Data.Clear();
        Rewind();
    }
    
    void COSEMType::Rewind()
    {
        m_pCurrentSchema = m_pSchema;
        m_Data.SetReadPosition(0);
        while (m_GetStates.size())
        {
            m_GetStates.pop();
        }
        while (m_AppendStates.size())
        {
            m_AppendStates.pop();
        }
    }
    
    bool COSEMType::Parse(DLMSVector * pData)
    {
        return m_Data.Append(pData) >= 0;
    }
    
#define CURRENT_APPEND_STATE m_AppendStates.top()
    bool COSEMType::SelectChoice(COSEMDataType Choice)
    {
        //
        // PRECONDITIONS
        //
        if (m_AppendStates.empty())
        {
            m_AppendStates.emplace(nullptr, ST_SIMPLE, Choice);
            return true;
        }
        //
        // The only time we can select a choice is if we are sitting within
        // a CHOICE section.
        //
        if (nullptr != CURRENT_APPEND_STATE.m_SchemaEntry)
        {
            CURRENT_APPEND_STATE.m_Choice = Choice;
            return true;
        }
        return false;
    }

#define CURRENT_GET_STATE m_GetStates.top()
    bool COSEMType::GetChoice(COSEMDataType * pChoice)
    {
        if (m_GetStates.size() && INVALID_CHOICE != CURRENT_GET_STATE.m_Choice)
        {
            *pChoice = CURRENT_GET_STATE.m_Choice;
            return true;
        }
        return false;

    }
    
    COSEMType::GetNextResult COSEMType::InternalSimpleGet(SchemaEntryPtr SchemaEntry, DLMSVariant * pValue)
    {
        GetNextResult RetVal = INVALID_CONDITION;
//        if (INTEGER_LIST_T == 
//           ASN_SCHEMA_INTERNAL_DATA_TYPE(SchemaEntry))
//        {
//            RetVal = GetINTEGER(SchemaEntry, pValue);
//        }
//        else
//        {
//            switch (ASN_SCHEMA_DATA_TYPE(SchemaEntry))
//            {
//            case INTEGER:
//                RetVal = GetINTEGER(SchemaEntry, pValue);
//                break;
//            case OBJECT_IDENTIFIER:
//                RetVal = ASNObjectIdentifier::Get(SchemaEntry, this, pValue) ? VALUE_RETRIEVED : INVALID_CONDITION;
//                break;
//            case GraphicString:
//            case OCTET_STRING:
//                RetVal = GetSTRING(SchemaEntry, pValue);
//                break;
//            case BIT_STRING:
//                RetVal = ASNBitString::Get(SchemaEntry, this, pValue) ? VALUE_RETRIEVED : INVALID_CONDITION;
//                break;
//                //
//                // TODO - Make Smarter and Smaller
//                //
//            case DT_Integer8:
//                RetVal = m_Data.Get<int8_t>(pValue) ? VALUE_RETRIEVED : INVALID_CONDITION;
//                break;
//            case DT_Unsigned8:
//                RetVal = m_Data.Get<uint8_t>(pValue) ? VALUE_RETRIEVED : INVALID_CONDITION;
//                break;
//            case DT_Integer16:
//                RetVal = m_Data.Get<int16_t>(pValue) ? VALUE_RETRIEVED : INVALID_CONDITION;
//                break;
//            case DT_Unsigned16:
//                RetVal = m_Data.Get<uint16_t>(pValue) ? VALUE_RETRIEVED : INVALID_CONDITION;
//                break;
//            case DT_Integer32:
//                RetVal = m_Data.Get<int32_t>(pValue) ? VALUE_RETRIEVED : INVALID_CONDITION;
//                break;
//            case DT_Unsigned32:
//                RetVal = m_Data.Get<uint32_t>(pValue) ? VALUE_RETRIEVED : INVALID_CONDITION;
//                break;
//            case DT_Integer64:
//                RetVal = m_Data.Get<int64_t>(pValue) ? VALUE_RETRIEVED : INVALID_CONDITION;
//                break;
//            case DT_Unsigned64:
//                RetVal = m_Data.Get<uint64_t>(pValue) ? VALUE_RETRIEVED : INVALID_CONDITION;
//                break;
//            case DT_Data:
//                {
//                    DLMSVector Vector;
//                    if (m_Data.GetVector(&Vector, m_Data.Size() - m_Data.GetReadPosition()))
//                    {
//                        pValue->set<DLMSVector>(Vector);
//                        RetVal = VALUE_RETRIEVED;
//                    }
//                }
//                break;
//            default:
//                throw std::out_of_range("InternalSimpleGet: Type not implemented.");
//            }
//        }
        return RetVal;
    }
    
    COSEMType::GetNextResult COSEMType::GetNextValue(DLMSValue * pValue)
    {
        GetNextResult       GetNextRetVal = INVALID_CONDITION;
//        DLMSSequence        Sequence;
//        //
//        // PRECONDITIONS
//        //
//        if (m_Data.Size() == 0)
//        {
//            return INVALID_CONDITION;
//        }
//        if (m_Data.IsAtEnd())
//        {
//            return END_OF_SCHEMA;
//        }
//        if (m_GetStates.empty())
//        {
//            m_GetStates.emplace(nullptr, ST_SIMPLE, INVALID_CHOICE);
//        }
//        while (VALUE_RETRIEVED == (GetNextRetVal = GetNextSchemaEntry(&CURRENT_GET_STATE.m_SchemaEntry)))
//        {
//            switch (CURRENT_GET_STATE.m_State)
//            {
//            case ST_SIMPLE:
//                if (BEGIN_CHOICE_T == 
//                    ASN_SCHEMA_INTERNAL_DATA_TYPE(CURRENT_GET_STATE.m_SchemaEntry))
//                {
//                    m_GetStates.emplace(nullptr, ST_CHOICE, INVALID_CHOICE);
//                    break;
//                }
//                else if (BEGIN_SEQUENCE_T == 
//                         ASN_SCHEMA_INTERNAL_DATA_TYPE(CURRENT_GET_STATE.m_SchemaEntry))
//                {
//                    m_GetStates.emplace(nullptr, ST_SEQUENCE, INVALID_CHOICE);
//                    break;
//                }
//                else
//                {
//                    DLMSVariant Value;
//                    GetNextRetVal = InternalSimpleGet(CURRENT_GET_STATE.m_SchemaEntry, &Value);
//                    if (VALUE_RETRIEVED == GetNextRetVal)
//                    {
//                        *pValue = Value;
//                    }
//                    return GetNextRetVal;
//                }
//                return SCHEMA_MISMATCH;
//            case ST_CHOICE:
//                if (END_CHOICE_T ==  
//                    ASN_SCHEMA_INTERNAL_DATA_TYPE(CURRENT_GET_STATE.m_SchemaEntry))
//                {
//                    m_GetStates.pop();
//                    break;
//                }
//                else if (BEGIN_CHOICE_ENTRY_T == ASN_SCHEMA_INTERNAL_DATA_TYPE(CURRENT_GET_STATE.m_SchemaEntry) &&
//                         (ASN_IS_IMPLICIT(CURRENT_GET_STATE.m_SchemaEntry) || (m_Data.PeekByte() & 0x80)))
//                {
//                    int8_t Choice = m_Data.Get<uint8_t>() & 0b00011111;
//                    if (ASN_SCHEMA_DATA_TYPE_SIZE(CURRENT_GET_STATE.m_SchemaEntry) == Choice)
//                    {
//                        m_GetStates.emplace(nullptr, ST_CHOICE_ENTRY, Choice);
//                        break;
//                    }
//                }
//                return SCHEMA_MISMATCH;
//            case ST_CHOICE_ENTRY:
//                if (END_CHOICE_ENTRY_T == 
//                    ASN_SCHEMA_INTERNAL_DATA_TYPE(CURRENT_GET_STATE.m_SchemaEntry))
//                {
//                    m_GetStates.pop();
//                    break;
//                }
//                else if (BEGIN_CHOICE_T == 
//                         ASN_SCHEMA_INTERNAL_DATA_TYPE(CURRENT_GET_STATE.m_SchemaEntry))
//                {
//                    m_GetStates.emplace(nullptr, ST_CHOICE, INVALID_CHOICE);
//                    break;
//                }
//                else if (BEGIN_SEQUENCE_T == 
//                         ASN_SCHEMA_INTERNAL_DATA_TYPE(CURRENT_GET_STATE.m_SchemaEntry))
//                {
//                    m_GetStates.emplace(nullptr, ST_SEQUENCE, INVALID_CHOICE);
//                    break;
//                }
//                else
//                {
//                    DLMSVariant Value;
//                    if (ASN_SCHEMA_OPTIONS(CURRENT_GET_STATE.m_SchemaEntry) & CONSTRUCTED)
//                    {
//                        // TODO
//                        m_Data.Skip(sizeof(uint8_t));
//                    }
//                    GetNextRetVal = InternalSimpleGet(CURRENT_GET_STATE.m_SchemaEntry, &Value);
//                    if (VALUE_RETRIEVED == GetNextRetVal)
//                    {
//                        *pValue = Value;
//                    }
//                    return GetNextRetVal;
//                }
//                return SCHEMA_MISMATCH;
//            case ST_SEQUENCE:
//                if (END_SEQUENCE_T == ASN_SCHEMA_INTERNAL_DATA_TYPE(CURRENT_GET_STATE.m_SchemaEntry))
//                {
//                    m_GetStates.pop();
//                    *pValue = Sequence;
//                    if (Sequence.size())
//                    {
//                        return VALUE_RETRIEVED; 
//                    }
//                    return NO_VALUE_FOUND;
//                }
//                else if (BEGIN_CHOICE_T == ASN_SCHEMA_INTERNAL_DATA_TYPE(CURRENT_GET_STATE.m_SchemaEntry))
//                {
//                    m_GetStates.emplace(nullptr, ST_CHOICE, INVALID_CHOICE);
//                    break;
//                }
//                else
//                {
//                    DLMSVariant Value;
//                    if (ASN_SCHEMA_OPTIONS(CURRENT_GET_STATE.m_SchemaEntry) & CONSTRUCTED)
//                    {
//                        // TODO
//                        m_Data.Skip(sizeof(uint8_t));
//                    }
//                    GetNextRetVal = InternalSimpleGet(CURRENT_GET_STATE.m_SchemaEntry, &Value);
//                    if (VALUE_RETRIEVED == GetNextRetVal)
//                    {
//                        Sequence.push_back(Value);
//                        break;
//                    }
//                }
//                return SCHEMA_MISMATCH;
//            default:
//                throw std::out_of_range("GetNextValue Not implemented.");
//            }
//        }
        return GetNextRetVal;
    }
    
//    COSEMType::GetNextResult COSEMType::GetNextValue(COSEMType * pValue)
//    {
//        GetNextResult  RetVal = INVALID_CONDITION;
//        DLMSValue      Value;
//        //
//        // PRECONDITIONS
//        //
//        if (m_Data.Size() == 0)
//        {
//            return RetVal;
//        }
//        
//        SchemaEntryPtr SchemaEntry = GetCurrentSchemaEntry();
//        switch (ASN_SCHEMA_DATA_TYPE(SchemaEntry))
//        {
//        case OBJECT_IDENTIFIER:
//        case INTEGER:
//        case OCTET_STRING:
//        case BIT_STRING:
//        case GraphicString:
//        case DT_Integer8:
//        case DT_Unsigned8:
//        case DT_Integer16:
//        case DT_Unsigned16:
//        case DT_Integer32:
//        case DT_Unsigned32:
//        case DT_Integer64:
//        case DT_Unsigned64:
//            RetVal = GetNextValue(&Value);
//            if (VALUE_RETRIEVED == RetVal)
//            {
//                pValue->SetSchemaType(SchemaEntry->m_SchemaType);
//                if (!pValue->Append(Value.get<DLMSVariant>()))
//                {
//                    RetVal = INVALID_CONDITION;
//                }
//                else
//                {
//                    pValue->Rewind();
//                }
//            }
//            break;
//        default:
//            throw std::out_of_range("GetNextValue Not Implemented");
//            break;
//        }
//        return RetVal;
//    }
    
    bool COSEMType::Append(const DLMSValue& Value)
    {
        return InternalAppend(Value);
    }
    
    bool COSEMType::Append()
    {
        //
        // NULL-DATA Case
        //
        return InternalAppend(DLMSVariant(blank()));
    }
    //
    // Operators
    //
    bool COSEMType::operator==(const std::vector<uint8_t>& rhs) const
    {
        return rhs == m_Data.GetBytes();
    }
    
    bool COSEMType::operator==(const COSEMType& rhs) const
    {
        return rhs.m_Data == m_Data;
    }
    //
    // Protected Methods
    //
    void COSEMType::SetSchemaType(SchemaBaseType DT)
    {
        m_pSchema = m_SingleDataType;
        m_SingleDataType[0].m_SchemaType = DT;
        Clear();
    }
    
    COSEMType::GetNextResult COSEMType::GetNextSchemaEntry(SchemaEntryPtr * ppSchemaEntry)
    {
        *ppSchemaEntry = m_pCurrentSchema;
        if (nullptr == m_pCurrentSchema)
        {
            return INVALID_CONDITION;
        }
        if (END_SCHEMA_T != m_pCurrentSchema->m_SchemaType)
        {
            ++m_pCurrentSchema;
            return VALUE_RETRIEVED;
        }
        return END_OF_SCHEMA;
    }

    class COSEMTypeAppendVisitor 
    {
    public:
        COSEMTypeAppendVisitor() = delete;
        COSEMTypeAppendVisitor(SchemaEntryPtr SchemaEntry, DLMSVector * pVector)
            : m_SchemaEntry(SchemaEntry), m_pVector(pVector)
        {
        }
        
        void operator()(const blank& Value)
        {
        }
        void operator()(const bool& Value)
        {
            IntegralAppend(Value ? 1 : 0);
        }
        void operator()(const int8_t& Value)
        {
            IntegralAppend(Value);
        }
        void operator()(const uint8_t& Value)
        {
            IntegralAppend(Value);
        }
        void operator()(const int16_t& Value)
        {
            IntegralAppend(Value);
        }
        void operator()(const uint16_t& Value)
        {
            IntegralAppend(Value);
        }
        void operator()(const int32_t& Value)
        {
            IntegralAppend(Value);
        }
        void operator()(const uint32_t& Value)
        {
            IntegralAppend(Value);
        }
        void operator()(const int64_t& Value)
        {
            IntegralAppend(Value);
        }
        void operator()(const uint64_t& Value)
        {
            IntegralAppend(Value);
        }
        void operator()(const std::string& Value)
        {
            switch (COSEM_SCHEMA_DATA_TYPE(m_SchemaEntry))
            {
            case NULL_DATA:
                break;
            case VISIBLE_STRING:
                ASNType::AppendLength(Value.length(), m_pVector);
                m_pVector->Append(Value);
                break;
            default:
                throw std::logic_error("Not implemented");
            }
        }
        void operator()(const double& Value)
        {
            FloatingPointAppend(Value);
        }
        void operator()(const DLMSVector& Value)
        {
            switch (COSEM_SCHEMA_DATA_TYPE(m_SchemaEntry))
            {
            case NULL_DATA:
                break;
            case BIT_STRING:
                m_pVector->Append(Value);
                break;
            case OCTET_STRING:
                {
                    size_t Length = Value.Size();
                    ASNType::AppendLength(Length, m_pVector);
                    m_pVector->Append(Value, 0, Length);
                }
                break;                
            default:
                throw std::logic_error("Not implemented");
            }
        }
        void operator()(const std::initializer_list<uint32_t>& Value)
        {
            throw std::logic_error("Not implemented");
        }
        void operator()(const DLMSBitSet& Value)
        {
            switch (COSEM_SCHEMA_DATA_TYPE(m_SchemaEntry))
            {
            case NULL_DATA:
                break;
            case BIT_STRING:
                {
                    size_t LengthIndex = 0;
                    LengthIndex = m_pVector->Append<uint8_t>(0);
                    ASNBitString Conversion(COSEM_SCHEMA_DATA_TYPE_SIZE(m_SchemaEntry), 
                        Value);
                    m_pVector->Append(Conversion.GetBytes());
                    (*m_pVector)[LengthIndex] = m_pVector->Size() - LengthIndex - 1;
                }
                break;
            default:
                throw std::logic_error("Not implemented");
            }
        }
        
    protected:
        void IntegralAppend(uint64_t VectorValue)
        {
            switch (COSEM_SCHEMA_DATA_TYPE(m_SchemaEntry))
            {
            case NULL_DATA:
                break;
            case UNSIGNED:
            case BOOLEAN:
                m_pVector->Append<uint8_t>(VectorValue);
                break;
            case DOUBLE_LONG:
                m_pVector->Append<int32_t>(VectorValue);
                break;
            case DOUBLE_LONG_UNSIGNED:
                m_pVector->Append<uint32_t>(VectorValue);
                break;
            case INTEGER:
                m_pVector->Append<int8_t>(VectorValue);
                break;
            case LONG:
                m_pVector->Append<int16_t>(VectorValue);
                break;
            case LONG_UNSIGNED:
                m_pVector->Append<uint16_t>(VectorValue);
                break;
            case LONG64:
                m_pVector->Append<int64_t>(VectorValue);
                break;
            case LONG64_UNSIGNED:
                m_pVector->Append<uint64_t>(VectorValue);
                break;
            default:
                throw std::logic_error("Not implemented");
            }
        }
        
        void FloatingPointAppend(double VectorValue)
        {
            switch (COSEM_SCHEMA_DATA_TYPE(m_SchemaEntry))
            {
            case NULL_DATA:
                break;
            //        case FLOAT32:
            //        case FLOAT64:
            default:
                throw std::logic_error("Not implemented");
            }
        }
        
        DLMSVector *   m_pVector;
        SchemaEntryPtr m_SchemaEntry;
    };
    
    bool COSEMType::InternalSimpleAppend(SchemaEntryPtr SchemaEntry, const DLMSVariant& Value)
    {
        m_Data.Append(uint8_t(COSEM_SCHEMA_DATA_TYPE(SchemaEntry)));
        try
        {
            COSEMTypeAppendVisitor Visitor(SchemaEntry, &m_Data);
            apply_visitor(Visitor, Value);
        }
        catch (const std::exception&)
        {
            return false;
        }
        return true;
    }
    
    bool COSEMType::InternalAppend(const DLMSValue& Value)
    {
        if (m_AppendStates.empty())
        {
            m_AppendStates.emplace(nullptr, ST_SIMPLE, INVALID_CHOICE);
        }
        while (VALUE_RETRIEVED == GetNextSchemaEntry(&CURRENT_APPEND_STATE.m_SchemaEntry))
        {
            switch (CURRENT_APPEND_STATE.m_State)
            {
            case ST_SIMPLE:
                if (BEGIN_CHOICE_T == 
                    COSEM_SCHEMA_INTERNAL_DATA_TYPE(CURRENT_APPEND_STATE.m_SchemaEntry))
                {
                    m_AppendStates.emplace(nullptr, ST_CHOICE, CURRENT_APPEND_STATE.m_Choice);
                    break;
                }
                else if (COSEM_IS_STRUCTURE_BEGIN(CURRENT_APPEND_STATE.m_SchemaEntry))
                {
                    m_AppendStates.emplace(nullptr, ST_STRUCTURE, CURRENT_APPEND_STATE.m_Choice);
                    break;
                }
                else 
                {
                    return InternalSimpleAppend(CURRENT_APPEND_STATE.m_SchemaEntry, Value.get<DLMSVariant>());                  
                }
                return false;
            case ST_CHOICE:
                if (END_CHOICE_T ==  
                    COSEM_SCHEMA_INTERNAL_DATA_TYPE(CURRENT_APPEND_STATE.m_SchemaEntry))
                {
                    m_AppendStates.pop();
                }
                else if (CURRENT_APPEND_STATE.m_Choice == COSEM_SCHEMA_DATA_TYPE(CURRENT_APPEND_STATE.m_SchemaEntry))
                {
                    if ((COSEMDataType::STRUCTURE == CURRENT_APPEND_STATE.m_Choice) &&
                        COSEM_IS_STRUCTURE_BEGIN(CURRENT_APPEND_STATE.m_SchemaEntry))
                    {
                        m_AppendStates.emplace(nullptr, ST_STRUCTURE, CURRENT_APPEND_STATE.m_Choice);
                        break;
                    }
                    else
                    {
                        return InternalSimpleAppend(CURRENT_APPEND_STATE.m_SchemaEntry, Value.get<DLMSVariant>());
                    }
                }
                break;
            case ST_STRUCTURE:
                if (COSEM_IS_STRUCTURE_END(CURRENT_APPEND_STATE.m_SchemaEntry))
                {
                    m_AppendStates.pop();
                    return true;
                }
                else if (BEGIN_CHOICE_T == COSEM_SCHEMA_INTERNAL_DATA_TYPE(CURRENT_APPEND_STATE.m_SchemaEntry))
                {
                    m_AppendStates.emplace(nullptr, ST_CHOICE, CURRENT_APPEND_STATE.m_Choice);
                    break;
                }
                else
                {
                    if (IsSequence(Value))
                    {
                        const DLMSSequence& Sequence = DLMSValueGetSequence(Value);
                        for (DLMSSequence::const_iterator it = Sequence.begin();
                             it != Sequence.end(); ++it)
                        {
                            //Appended = InternalSimpleAppend(CURRENT_APPEND_STATE.m_SchemaEntry, *it);
                        }
                    }
                    else
                    {
//                        Appended = InternalSimpleAppend(CURRENT_APPEND_STATE.m_SchemaEntry, Value.get<DLMSVariant>());
//                        if (Appended)
//                        {
//                            return true;
//                        }
                    }
                }
                return false;
                
            default:
                return false;
            }
        }
        return false;
    }

    bool COSEMType::InternalAppend(const DLMSVector& Value)
    {
        return m_Data.Append(Value) >= 0;
    }
    //
    // COSEMBitString
    //
    COSEMBitString::COSEMBitString()
        : COSEMType(BIT_STRING)
    {
    }
        
    COSEMBitString::~COSEMBitString()
    {
    }
        
    COSEMBitString::COSEMBitString(size_t BitsExpected)
        : COSEMType(BIT_STRING), m_BitsExpected(BitsExpected)
    {
        if (0 == BitsExpected)
        {
            throw std::invalid_argument("BitsExpected must be > 0");
        }
    }
    
    COSEMBitString::COSEMBitString(size_t BitsExpected, const DLMSBitSet& Value)
        : COSEMType(BIT_STRING), m_BitsExpected(BitsExpected)
    {
        if (0 == BitsExpected)
        {
            throw std::invalid_argument("BitsExpected must be > 0");
        }
        uint8_t ByteOffset = BitsExpected / 8;
        uint8_t ValueBitIndex = 0;
        if (0 == ByteOffset)
        {
            ++ByteOffset;
        }
        m_Data.Append<uint8_t>(8 - (BitsExpected % 8));
        for (uint8_t ByteIndex = 0; ByteIndex < ByteOffset; ++ByteIndex)
        {
            uint8_t CurrentByte = 0;
            for (int BitShift = 7; BitShift >= 0; BitShift--)
            {
                CurrentByte |= (Value[ValueBitIndex++] << BitShift);
            }
            m_Data.Append<uint8_t>(CurrentByte);
        }
        
    }
    
    COSEMBitString::operator DLMSVariant() const
    {
        return m_Data;
    }
    
}