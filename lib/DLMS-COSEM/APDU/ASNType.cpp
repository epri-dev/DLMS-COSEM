#include <algorithm>
#include <set>

#include "APDU/ASNType.h"

namespace EPRI
{
    namespace ASN
    {
        // 
        // Default Basic Schemas
        //
        ASN_BEGIN_SCHEMA(EmptySchema)
        ASN_END_SCHEMA
        ASN_BEGIN_SCHEMA(OctetStringSchema)
            ASN_OCTET_STRING_TYPE(ASN::NO_OPTIONS)
        ASN_END_SCHEMA
        ASN_BEGIN_SCHEMA(ObjectIdentifierSchema)
            ASN_OBJECT_IDENTIFIER_TYPE(ASN::NO_OPTIONS)
        ASN_END_SCHEMA
        ASN_BEGIN_SCHEMA(IntegerSchema)
            ASN_INTEGER_TYPE(ASN::NO_OPTIONS)
        ASN_END_SCHEMA
        ASN_BEGIN_SCHEMA(GraphicStringSchema)
            ASN_GraphicString_TYPE(ASN::NO_OPTIONS)
        ASN_END_SCHEMA
        ASN_BEGIN_SCHEMA(ImplicitOctetStringSchema)
            ASN_OCTET_STRING_TYPE(ASN::IMPLICIT)
        ASN_END_SCHEMA
        ASN_BEGIN_SCHEMA(ImplicitObjectIdentifierSchema)
            ASN_OBJECT_IDENTIFIER_TYPE(ASN::IMPLICIT)
        ASN_END_SCHEMA
        
    }
    //
    // Global Functions
    //
    
    //
    // ASNType
    //
    ASNType::ASNType()
    {
    }

    ASNType::ASNType(ASN::SchemaEntryPtr Schema) : 
        m_pSchema(Schema),
        m_pCurrentSchema(Schema)
    {
    }
    
    ASNType::ASNType(ASN::SchemaBaseType DT) :
        m_pSchema(m_SingleDataType),
        m_pCurrentSchema(m_SingleDataType)
    {
        m_SingleDataType[0].m_SchemaType = DT;
    }
    
    ASNType::ASNType(ASN::SchemaBaseType DT, const DLMSVariant& Value) :
        m_pSchema(m_SingleDataType),
        m_pCurrentSchema(m_SingleDataType)
    {
        m_SingleDataType[0].m_SchemaType = DT;
        InternalAppend(Value);
        Rewind();
    }

    ASNType::~ASNType()
    {
    }
        
    std::vector<uint8_t> ASNType::GetBytes(ASN::TagIDType Tag, ASN::ComponentOptionType Options)
    {
        DLMSVector  RetVal;
        size_t      Position = 0;
        size_t      Length = m_Data.Size();
        //
        // If this is optional or default and not data, then
        // we have nothing to give
        //
        if ((nullptr == m_pSchema) ||
            ((Options & ASN::OPTIONAL) && 
             (0 == Length)))
        {
            return std::vector<uint8_t>();
        }
        //
        // Outer tag
        RetVal.Append<uint8_t>(ASN_MAKE_TAG(Tag, Options));
        //
        // Length   
        if (!(Options & ASN::CONSTRUCTED))
        {
            if (ASN::OBJECT_IDENTIFIER == m_pSchema->m_SchemaType)
            {
                Length -= 2;
                Position += 2;
            }
        }
        //
        // Length
        if (AppendLength(Length, &RetVal))
        {
            //
            // Data
            RetVal.Append(m_Data,
                          Position,
                          Length);
        }
        return RetVal.GetBytes();
    }
    
    std::vector<uint8_t> ASNType::GetBytes() const
    {
        return m_Data.GetBytes();
    }
        
    bool ASNType::IsEmpty() const
    {
        return m_Data.Size() == 0;
    }

    void ASNType::Clear()
    {
        m_Data.Clear();
        Rewind();
    }
    
    void ASNType::Rewind()
    {
        m_pCurrentSchema = m_pSchema;
        m_AppendState = ST_SIMPLE;
        m_GetState = ST_SIMPLE;
        m_Choice = INVALID_CHOICE;
        m_Data.SetReadPosition(0);
    }
    
    bool ASNType::SelectChoice(int8_t Choice)
    {
        // The only time we can select a choice is if we are sitting within
        // a CHOICE section.
        //
        ASN::SchemaEntryPtr pSchemaEntry = GetCurrentSchemaEntry();
        if (nullptr != pSchemaEntry &&
            (ASN::BEGIN_CHOICE_T == ASN_SCHEMA_INTERNAL_DATA_TYPE(pSchemaEntry) ||
             ASN::BEGIN_CHOICE_ENTRY_T == ASN_SCHEMA_INTERNAL_DATA_TYPE(pSchemaEntry)))
        {
            m_Choice = Choice;
            return true;
        }
        return false;
    }
    
    bool ASNType::GetChoice(int8_t * pChoice)
    {
        if (INVALID_CHOICE != m_Choice)
        {
            *pChoice = m_Choice;
            return true;
        }
        return false;
    }
    
    ASNType::GetNextResult ASNType::GetINTEGER(ASN::SchemaEntryPtr SchemaEntry, DLMSVariant * pValue)
    {
        GetNextResult RetVal = NO_VALUE_FOUND;
        bool          GetResult = false;
        //
        // Validate our appropriate lengths for this data type.
        //
        const std::set<uint8_t> VALID_LENGTHS({ 1, 2, 4, 8 });
        if (m_Data.PeekByte() == ASN::INTEGER &&
            VALID_LENGTHS.find(m_Data.PeekByte(1)) != VALID_LENGTHS.end())
        {
            m_Data.Skip(sizeof(uint8_t));
            switch (m_Data.Get<uint8_t>())
            {
            case 1:
                GetResult = m_Data.Get<int8_t>(pValue);
                break;
            case 2:
                GetResult = m_Data.Get<int16_t>(pValue);
                break;
            case 4:
                GetResult = m_Data.Get<int32_t>(pValue);
                break;
            case 8:
                GetResult = m_Data.Get<int64_t>(pValue);
                break;
            default:
                RetVal = INVALID_CONDITION;
                break;                        
            }
            if (GetResult)
            {
                RetVal = VALUE_RETRIEVED;
            }
        }
        return RetVal;
    }
    
    ASNType::GetNextResult ASNType::GetSTRING(ASN::SchemaEntryPtr SchemaEntry, DLMSVariant * pValue)
    {
        GetNextResult RetVal = INVALID_STREAM;
        bool          IsImplicit = ASN_IS_IMPLICIT(SchemaEntry);
        //
        // Validate our appropriate lengths for this data type.
        //
        if (IsImplicit ||
            ((ASN_SCHEMA_DATA_TYPE(SchemaEntry) == ASN::OCTET_STRING) && (m_Data.PeekByte() == ASN::OCTET_STRING)) ||
            ((ASN_SCHEMA_DATA_TYPE(SchemaEntry) == ASN::GraphicString) && (m_Data.PeekByte() == ASN::GraphicString)))
        {
            size_t Length = 0;
            if (!IsImplicit)
            {
                m_Data.Skip(sizeof(uint8_t));
            }
            if (GetLength(&m_Data, &Length))
            {
                switch (ASN_SCHEMA_DATA_TYPE(SchemaEntry))
                {
                case ASN::OCTET_STRING:
                    {
                        DLMSVector Vector;
                        if (m_Data.GetVector(&Vector, Length))
                        {
                            pValue->set<DLMSVector>(Vector);
                            RetVal = VALUE_RETRIEVED;
                        }
                    }
                    break;
                case ASN::GraphicString:
                    {
                        std::string String;
                        if (m_Data.Get(&String, Length))
                        {
                            pValue->set<std::string>(String);
                            RetVal = VALUE_RETRIEVED;
                        }
                    }
                    break;
                default:
                    break;
                }
            }
        }
        return RetVal;
    }
    
    ASNType::GetNextResult ASNType::InternalSimpleGet(ASN::SchemaEntryPtr SchemaEntry, DLMSVariant * pValue)
    {
        GetNextResult RetVal = INVALID_CONDITION;
        if(ASN::INTEGER_LIST_T == 
           ASN_SCHEMA_INTERNAL_DATA_TYPE(SchemaEntry))
        {
            RetVal = GetINTEGER(SchemaEntry, pValue);
        }
        else
        {
            switch (ASN_SCHEMA_DATA_TYPE(SchemaEntry))
            {
            case ASN::INTEGER:
                RetVal = GetINTEGER(SchemaEntry, pValue);
                break;
            case ASN::OBJECT_IDENTIFIER:
                RetVal = ASNObjectIdentifier::Get(SchemaEntry, this, pValue) ? VALUE_RETRIEVED : INVALID_CONDITION;
                break;
            case ASN::GraphicString:
            case ASN::OCTET_STRING:
                RetVal = GetSTRING(SchemaEntry, pValue);
                break;
            case ASN::BIT_STRING:
                RetVal = ASNBitString::Get(SchemaEntry, this, pValue) ? VALUE_RETRIEVED : INVALID_CONDITION;
                break;
            default:
                break;
            }
        }
        return RetVal;
    }
    
    ASNType::GetNextResult ASNType::GetNextValue(DLMSVariant * pValue)
    {
        ASN::SchemaEntryPtr SchemaEntry;
        bool                Gotten = false;
        GetNextResult       GetNextRetVal = INVALID_CONDITION;
        //
        // PRECONDITIONS
        //
        if (m_Data.Size() == 0)
        {
            return INVALID_CONDITION;
        }
        while (VALUE_RETRIEVED == (GetNextRetVal = GetNextSchemaEntry(&SchemaEntry)))
        {
            switch (m_GetState)
            {
            case ST_SIMPLE:
                if (ASN::BEGIN_CHOICE_T == 
                    ASN_SCHEMA_INTERNAL_DATA_TYPE(SchemaEntry))
                {
                    m_GetState = ST_CHOICE;
                    m_Choice = INVALID_CHOICE;
                    break;
                }
                else
                {
                    return InternalSimpleGet(SchemaEntry, pValue);
                }
                break;
            case ST_CHOICE:
                if (ASN::END_CHOICE_T ==  
                    ASN_SCHEMA_INTERNAL_DATA_TYPE(SchemaEntry))
                {
                    m_GetState = ST_SIMPLE;  
                    if (Gotten)
                    {
                        return VALUE_RETRIEVED;
                    }
                    else
                    {
                        m_Choice = INVALID_CHOICE;
                    }
                    return NO_VALUE_FOUND;
                }
                else if (ASN::BEGIN_CHOICE_ENTRY_T == ASN_SCHEMA_INTERNAL_DATA_TYPE(SchemaEntry) &&
                         (m_Data.PeekByte() & 0x80))
                {
                    int8_t Choice = m_Data.PeekByte() & 0b00011111;
                    if (ASN_SCHEMA_DATA_TYPE_SIZE(SchemaEntry) == Choice)
                    {
                        m_GetState = ST_CHOICE_ENTRY;  
                        m_Choice = Choice;
                    }
                }
                else if (!Gotten)
	            {
    	            m_GetState = ST_SIMPLE;  
    	            m_Choice = INVALID_CHOICE;
    	            return SCHEMA_MISMATCH;
	            }
                break;
            case ST_CHOICE_ENTRY:
                if (!Gotten)
                {
                    m_Data.Skip(sizeof(uint8_t));
                    if (ASN_SCHEMA_OPTIONS(SchemaEntry) & ASN::CONSTRUCTED)
                    {
                        // TODO
                        m_Data.Skip(sizeof(uint8_t));
                    }
                    if (VALUE_RETRIEVED == InternalSimpleGet(SchemaEntry, pValue))
                    {
                        Gotten = true;
                    }
                }
                else if (ASN::END_CHOICE_ENTRY_T == ASN_SCHEMA_INTERNAL_DATA_TYPE(SchemaEntry))
                {
                    m_GetState = ST_CHOICE;
                }
                break;
            default:
                throw std::out_of_range("GetNextValue Not implemented.");
            }
        }
        return GetNextRetVal;
    }

    ASNType::GetNextResult ASNType::GetNextValue(ASNType * pValue)
    {
        GetNextResult  RetVal = INVALID_CONDITION;
        DLMSVariant    Variant;
        //
        // PRECONDITIONS
        //
        if (m_Data.Size() == 0)
        {
            return RetVal;
        }
        
        ASN::SchemaEntryPtr SchemaEntry = GetCurrentSchemaEntry();
        switch (ASN_SCHEMA_DATA_TYPE(SchemaEntry))
        {
        case ASN::OBJECT_IDENTIFIER:
        case ASN::INTEGER:
        case ASN::OCTET_STRING:
        case ASN::BIT_STRING:
        case ASN::GraphicString:
            RetVal = GetNextValue(&Variant);
            if (VALUE_RETRIEVED == RetVal)
            {
                pValue->SetSchemaType(SchemaEntry->m_SchemaType);
                if (!pValue->Append(Variant))
                {
                    RetVal = INVALID_CONDITION;
                }
                else
                {
                    pValue->Rewind();
                }
            }
            break;
        default:
            throw std::out_of_range("GetNextValue Not Implemented");
            break;
        }
        return RetVal;
    }
    
    bool ASNType::Append(const DLMSVariant& Value)
    {
        return InternalAppend(Value);
    }
    
    bool ASNType::Append(ASNType * pValue)
    {
        return InternalAppend(pValue);
    }

    bool ASNType::AppendLength(size_t Length, DLMSVector * pData)
    {
        if (Length > 0x7F)
        {
            uint8_t Buffer[sizeof(size_t)];
            uint8_t WorkingLength = 0;
            while (Length > 0)
            {
                WorkingLength++;
                if (WorkingLength > sizeof(Buffer))
                {
                    return false;
                }
                Buffer[sizeof(Buffer) - WorkingLength] = (0xFF & Length);
                Length = Length >> 8;
            }
            pData->Append<uint8_t>(0x80 | WorkingLength);
            pData->AppendBuffer(Buffer + sizeof(Buffer) - WorkingLength, 
                WorkingLength);
        }
        else
        {
            pData->Append<uint8_t>(Length);
        }
        return true;
    }
    
    uint8_t ASNType::CalculateLengthBytes(size_t Length)
    {
        return (Length / 0x7F) + 1;
    }
    
    bool ASNType::GetLength(DLMSVector * pData, size_t * pLength)
    {
        size_t LengthBytes = 0;
        if (PeekLength(*pData, 0, pLength, &LengthBytes))
        {
            pData->Skip(LengthBytes);
            return true;
        }
        return false;
    }
    
    bool ASNType::PeekLength(const DLMSVector& Data, size_t Offset, size_t * pLength, size_t * pBytes /* = nullptr */)
    {
        uint8_t LengthSize = 0;
        size_t  Index = Offset;
        //
        // PRECONDITIONS
        //
        if (Data.IsAtEnd())
        {
            return false;
        }
        if (Data.Peek<uint8_t>(Index) & 0x80)
        {
            LengthSize = Data.Peek<uint8_t>(Index++) & 0x7F;
            if (0 == (LengthSize & 0x7F) ||
                LengthSize > sizeof(size_t))
            {
                return false;
            }
        }
        else
        {
            LengthSize = 1;
        }
        
        if (nullptr != pBytes)
        {
            *pBytes = LengthSize;
        }
        
        *pLength = 0;
        while (LengthSize > 0)
        {
            if (Data.GetReadPosition() + Index > Data.Size())
            {
                return false;
            }
            
            *pLength <<= 8;
            *pLength |= Data.Peek<uint8_t>(Index++);

            LengthSize--;
        }
        return true;       
    }
    //
    // Operators
    //
    bool ASNType::operator==(const std::vector<uint8_t>& rhs) const
    {
        return rhs == m_Data.GetBytes();
    }
    
    bool ASNType::operator==(const ASNType& rhs) const
    {
        return rhs.m_Data == m_Data;
    }
    //
    // Protected Methods
    //
    void ASNType::SetSchemaType(ASN::SchemaBaseType DT)
    {
        m_pSchema = m_SingleDataType;
        m_SingleDataType[0].m_SchemaType = DT;
        Clear();
    }
    
    ASNType::GetNextResult ASNType::GetNextSchemaEntry(ASN::SchemaEntryPtr * ppSchemaEntry)
    {
        *ppSchemaEntry = m_pCurrentSchema;
        if (nullptr == m_pCurrentSchema)
        {
            return INVALID_CONDITION;
        }
        if (ASN::END_SCHEMA_T != m_pCurrentSchema->m_SchemaType)
        {
            ++m_pCurrentSchema;
            return VALUE_RETRIEVED;
        }
        return END_OF_SCHEMA;
    }
    
    bool ASNType::InternalAppend(const DLMSVariant& Value)
    {
        bool                Continue = true;
        uint8_t             ChoiceIndex = 0;
        ASN::SchemaEntryPtr SchemaEntry;
       
        while (VALUE_RETRIEVED == GetNextSchemaEntry(&SchemaEntry))
        {
            switch (m_AppendState)
            {
            case ST_SIMPLE:
                if (ASN::BEGIN_CHOICE_T == 
                    ASN_SCHEMA_INTERNAL_DATA_TYPE(SchemaEntry))
                {
                    m_AppendState = ST_CHOICE;
                    break;
                }
                switch (ASN_SCHEMA_DATA_TYPE(SchemaEntry))
                {
                case ASN::GraphicString:
                    if (Value.which() == VAR_STRING)
                    {
                        AppendLength(Value.get<std::string>().length(), &m_Data);
                        m_Data.Append(Value.get<std::string>());
                        return true;
                    }
                    return false;
                case ASN::OCTET_STRING:
                    if (Value.which() == VAR_VECTOR)
                    {
                        m_Data.Append<uint8_t>(ASN::OCTET_STRING);
                        AppendLength(Value.get<DLMSVector>().Size(), &m_Data);
                        m_Data.Append(Value.get<DLMSVector>());
                        return true;
                    }
                    return false;
                case ASN::INTEGER:
                    {
                        m_Data.Append<uint8_t>(ASN_SCHEMA_DATA_TYPE(SchemaEntry));
                        size_t LengthIndex = m_Data.Append<uint8_t>(0);
                        m_Data.Append(Value);
                        m_Data[LengthIndex] = m_Data.Size() - LengthIndex - 1;
                    }
                    return true;
                case ASN::OBJECT_IDENTIFIER:
                    m_Data.Append(Value);
                    return true;
                case ASN::BIT_STRING:
                    {
                        bool   NeedLength = ASN_IS_IMPLICIT(SchemaEntry);
                        size_t LengthIndex = 0;
                        if (NeedLength)
                        {
                            m_Data.Append<uint8_t>(ASN_SCHEMA_DATA_TYPE(SchemaEntry));
                            LengthIndex = m_Data.Append<uint8_t>(0);
                        }
                        ASNBitString Conversion(ASN_SCHEMA_DATA_TYPE_SIZE(SchemaEntry), Value.get<DLMSBitSet>());
                        m_Data.Append(Conversion.GetBytes());
                        if (NeedLength)
                        {
                            m_Data[LengthIndex] = m_Data.Size() - LengthIndex - 1;
                        }
                    }
                    return true;
                case ASN::VOID:
                default:
                    break;
                }
                break;
            case ST_CHOICE:
                if (ASN::END_CHOICE_T == 
                    ASN_SCHEMA_INTERNAL_DATA_TYPE(SchemaEntry))
                {
                    ChoiceIndex = 0;
                    m_AppendState = ST_SIMPLE;  
                }
                break;
            case ST_SEQUENCE:
                break;
            }
        }
        return false;
    }
    
    bool ASNType::InternalSimpleAppend(ASN::SchemaEntryPtr SchemaEntry, ASNType * pValue)
    {
        if (nullptr == pValue->m_pSchema)
        {
            return false;
        }
        
        if (ASN::INTEGER_LIST_T == 
            ASN_SCHEMA_INTERNAL_DATA_TYPE(SchemaEntry))
        {
            if (ASN::INTEGER == pValue->GetCurrentSchemaType() &&
                SchemaEntry->m_Extra.which() == VAR_INIT_LIST)
            {
                DLMSVariant Current;
                if (VALUE_RETRIEVED == pValue->GetNextValue(&Current) &&
                    IsValueInVariant(Current, SchemaEntry->m_Extra))
                {
                    return InternalAppend(pValue->m_Data);
                }
            }
        }
        else if(ASN_SCHEMA_DATA_TYPE(SchemaEntry) ==
                    ASN_SCHEMA_DATA_TYPE(pValue->m_pSchema))
        {
            return InternalAppend(pValue->m_Data);
        }
        return false;
    }

    bool ASNType::InternalAppend(ASNType * pValue)
    {
        ASN::SchemaEntryPtr SchemaEntry;
        bool                Appended = false;
        
        while (VALUE_RETRIEVED == GetNextSchemaEntry(&SchemaEntry))
        {
            switch (m_AppendState)
            {
            case ST_SIMPLE:
                if (ASN::BEGIN_CHOICE_T == 
                    ASN_SCHEMA_INTERNAL_DATA_TYPE(SchemaEntry))
                {
                    m_AppendState = ST_CHOICE;
                    break;
                }
                else if (InternalSimpleAppend(SchemaEntry, pValue))
                {
                    return true;
                }
                return false;
            case ST_CHOICE:
                if (ASN::END_CHOICE_T ==  
                    ASN_SCHEMA_INTERNAL_DATA_TYPE(SchemaEntry))
                {
                    m_AppendState = ST_SIMPLE;  
                    m_Choice = INVALID_CHOICE;
                    
                    if (!Appended)
                    {
                        return false;
                    }
                    return true;
                }
                else if (ASN::BEGIN_CHOICE_ENTRY_T == ASN_SCHEMA_INTERNAL_DATA_TYPE(SchemaEntry) &&
                         m_Choice == ASN_SCHEMA_DATA_TYPE_SIZE(SchemaEntry))
                {
                    m_AppendState = ST_CHOICE_ENTRY;  
                }
                break;
            case ST_CHOICE_ENTRY:
                if (!Appended)
                {
                    bool Constructed = ASN_SCHEMA_OPTIONS(SchemaEntry) & ASN::CONSTRUCTED;
                    m_Data.Append<uint8_t>(0x80 | m_Choice | 
                        (Constructed ? 0b00100000 : 0x00));

                    //
                    // TODO - Handle real length...
                    //
                    ssize_t LengthIndex = -1;
                    if (Constructed)
                    {
                        LengthIndex = m_Data.Append<uint8_t>(0x00);
                    }
                    Appended = InternalSimpleAppend(SchemaEntry, pValue);
                    if (Constructed && Appended)
                    {
                        m_Data[LengthIndex] = m_Data.Size() - LengthIndex - sizeof(uint8_t);
                    }
                }
                else if (ASN::END_CHOICE_ENTRY_T == ASN_SCHEMA_INTERNAL_DATA_TYPE(SchemaEntry))
                {
                    m_AppendState = ST_CHOICE;
                }
                break;
            default:
                return false;
            }
        }
        return false;
    }
    
    bool ASNType::InternalAppend(const DLMSVector& Value)
    {
        return m_Data.Append(Value) >= 0;
    }

    //
    // ASNObjectIdentifier
    //
    ASNObjectIdentifier::ASNObjectIdentifier(ArcList List, ASN::ComponentOptions Options /* = ASN::NO_OPTIONS */, OIDType OT /* = ABSOLUTE */)
        : ASNType(ASN::OBJECT_IDENTIFIER | Options)
    {
        bool                Error = false;
        uint8_t             Buffer[sizeof(uintmax_t) + 2];
        uint8_t *           pEnd = &Buffer[sizeof(Buffer)];
        uint8_t *           pStart = pEnd;
        ArcList::iterator   it = List.begin();
        bool                Implicit = IsCurrentSchemaImplicit();
        
        if (!Implicit)
        {
            //
            // Tag
            //
            m_Data.Append<uint8_t>(OT);
            //
            // Reserve for length (no long encoding needed for OID)
            //
            m_Data.Append<uint8_t>(0);
        }
        if (ABSOLUTE == OT && List.size() >= 2)
        {
            uintmax_t FirstArc = *it++;
            m_Data.Append<uint8_t>((40 * FirstArc) + (*it++));
        }
        for (;it != List.end(); ++it)
        {
            uintmax_t CurrentArc = *it;
            do
            {
                if (pStart != &Buffer[0])
                {
                    --pStart;
                    *pStart = CurrentArc & 0x7F;
                    CurrentArc = CurrentArc >> 7;
                }
                else
                {
                    Error = true;
                    break;
                }

            } while (CurrentArc != 0);

            if (Error)
            {
                m_Data.Clear();
                break;
            }
                
            while (pStart != pEnd)
            {
                uint8_t Value = *pStart;
                ++pStart;
                if (pStart != pEnd)
                {
                    Value |= 0x80;
                }
                m_Data.Append<uint8_t>(Value);
            }
        }
        if (!Implicit)
        {
            m_Data[1] = uint8_t(m_Data.Size() - 2);
        }
    }

    ASNObjectIdentifier::~ASNObjectIdentifier()
    {
    }
    
    bool ASNObjectIdentifier::Peek(ASN::SchemaEntryPtr SchemaEntry, const ASNType& Value, DLMSVariant * pVariant, size_t * pBytes /* = nullptr*/)
    {
        DLMSVector Output;
        int        Byte = Value.m_Data.PeekByte();
        bool       CanAppend = false;
        bool       IsImplicit = ASN_IS_IMPLICIT(SchemaEntry);
        size_t     Offset = 0;
        size_t     Count = 0;
        
        if (!IsImplicit && (ABSOLUTE == Byte || RELATIVE == Byte))
        { 
            Output.Append<uint8_t>(Byte);
            Byte = Value.m_Data.PeekByte(1);
            if (Byte >= 0)
            {
                Output.Append<uint8_t>(Byte);
                Offset = Value.m_Data.GetReadPosition() + 2;
                Count = Byte;
                CanAppend = true;
            }
        }
        else if (IsImplicit)
        {
            CanAppend = true;
        }
        if (CanAppend)
        {
            if (Output.Append(Value.m_Data, 
                Offset,
                Count) >= 0)
            {
                pVariant->set<DLMSVector>(Output);
                if (pBytes)
                {
                    *pBytes = Output.Size();
                }
                return true;
            }
        }
        return false;
    }
    
    bool ASNObjectIdentifier::Get(ASN::SchemaEntryPtr SchemaEntry, ASNType * pValue, DLMSVariant * pVariant)
    {
        size_t Bytes = 0;
        if (Peek(SchemaEntry, *pValue, pVariant, &Bytes))
        {
            pValue->m_Data.Skip(Bytes);
            return true;
        }
        return false;
    }
    
    bool ASNObjectIdentifier::Get(ArcVector * pVector)
    {
        return false;
    }

    //
    // ASNBitString
    //
    ASNBitString::ASNBitString() :
        ASNType(ASN::BIT_STRING)
    {
    }
        
    ASNBitString::~ASNBitString()
    {
    }
        
    ASNBitString::ASNBitString(size_t BitsExpected)
        : ASNType(ASN::BIT_STRING), 
          m_BitsExpected(BitsExpected)
    {
        if (0 == BitsExpected)
        {
            throw std::invalid_argument("BitsExpected must be > 0");
        }
    }
    
    ASNBitString::ASNBitString(size_t BitsExpected, const DLMSBitSet& Value) :
        ASNType(ASN::BIT_STRING), 
        m_BitsExpected(BitsExpected)
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
    
    bool ASNBitString::Peek(ASN::SchemaEntryPtr SchemaEntry, const ASNType& Value, DLMSVariant * pVariant, size_t * pBytes /* = nullptr*/)
    {
        DLMSVector Output;
        int        Byte = Value.m_Data.PeekByte();
        size_t     Offset = 0;
        size_t     Length = 0;
        size_t     LengthBytes = 0;
        
        // Tag & Length (If not IMPLICIT)
        if (!ASN_IS_IMPLICIT(SchemaEntry) &&
            ASN::BIT_STRING == Byte &&
                PeekLength(Value.m_Data, 1, &Length, &LengthBytes))
        {
            Offset += LengthBytes + sizeof(uint8_t);
        }
        else
        {
            Length = Value.m_Data.Size();
        }

        // Bit Length & Byte Length
        if (pBytes != nullptr)
        {
            *pBytes = Offset + Length;
        }
        Length = (Length - sizeof(uint8_t)) * 8 - Byte;

        // Bits Remaining
        Byte = Value.m_Data.PeekByte(Offset++);
        if (Byte >= 8)
        {
            return false;
        }
            
        DLMSBitSet Bitset;
        for (size_t BitIndex = 0; BitIndex < Length; ++BitIndex)
        {
            Bitset[BitIndex] = Value.m_Data[Offset + (BitIndex / 8)] & 
                (1 << (8 - (BitIndex % 8) - 1));
        }
        *pVariant = Bitset;
        return true;
    }
    
    bool ASNBitString::Get(ASN::SchemaEntryPtr SchemaEntry, ASNType * pValue, DLMSVariant * pVariant)
    {
        size_t Bytes = 0;
        if (Peek(SchemaEntry, *pValue, pVariant, &Bytes))
        {
            pValue->m_Data.Skip(Bytes);
            return true;
        }
        return false;
    }
    //
    // ASNVoid
    //
    ASNVoid::ASNVoid()
        : ASNType(ASN::VOID)
    {
    }

    const ASNType ASNMissing = ASNVoid();    
        
}