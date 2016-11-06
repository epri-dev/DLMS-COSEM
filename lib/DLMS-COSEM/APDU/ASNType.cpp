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
    
    ASNType::ASNType(ASN::DataTypes DT) :
        m_pSchema(m_SingleDataType),
        m_pCurrentSchema(m_SingleDataType)
    {
        m_SingleDataType[0].m_SchemaType = DT;
    }
    
    ASNType::ASNType(ASN::DataTypes DT, const DLMSVariant& Value)
        :
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
    
    bool ASNType::GetINTEGER(DLMSVariant * pValue)
    {
        bool RetVal = false;
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
                RetVal = m_Data.Get<int8_t>(pValue);
                break;
            case 2:
                RetVal = m_Data.Get<int16_t>(pValue);
                break;
            case 4:
                RetVal = m_Data.Get<int32_t>(pValue);
                break;
            case 8:
                RetVal = m_Data.Get<int64_t>(pValue);
                break;
            default:
                RetVal = false;
                break;                        
            }
        }
        return RetVal;
    }
    
    ASNType::GetNextValueResult ASNType::InternalSimpleGet(ASN::SchemaEntryPtr SchemaEntry, DLMSVariant * pValue)
    {
        GetNextValueResult RetVal = INVALID_CONDITION;
        if(ASN::INTEGER_LIST_T == 
           ASN_SCHEMA_INTERNAL_DATA_TYPE(SchemaEntry))
        {
            if (GetINTEGER(pValue))
            {
                RetVal = VALUE_RETRIEVED;
            }
        }
        else
        {
            switch (ASN_SCHEMA_DATA_TYPE(SchemaEntry))
            {
            case ASN::INTEGER:
                RetVal = GetINTEGER(pValue) ? VALUE_RETRIEVED : INVALID_CONDITION;
                break;
            case ASN::OBJECT_IDENTIFIER:
                RetVal = ASNObjectIdentifier::Get(this, pValue) ? VALUE_RETRIEVED : INVALID_CONDITION;
                break;
            default:
                break;
            }
        }
        return RetVal;
    }
    
    ASNType::GetNextValueResult ASNType::GetNextValue(DLMSVariant * pValue)
    {
        ASN::SchemaEntryPtr SchemaEntry;
        bool                Gotten = false;
        //
        // PRECONDITIONS
        //
        if (m_Data.Size() == 0)
        {
            return INVALID_CONDITION;
        }
        while (GetNextSchemaEntry(&SchemaEntry))
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
                break;
            }
        }
        return INVALID_CONDITION;
    }

    ASNType::GetNextValueResult ASNType::GetNextValue(ASNType * pValue)
    {
        GetNextValueResult  RetVal = INVALID_CONDITION;
        DLMSVariant         Variant;
        //
        // PRECONDITIONS
        //
        if (m_Data.Size() == 0)
        {
            return RetVal;
        }
        
        ASN::DataTypes CurrentSchemaType = GetCurrentSchemaType();
        switch (CurrentSchemaType)
        {
        case ASN::OBJECT_IDENTIFIER:
        case ASN::INTEGER:
            RetVal = GetNextValue(&Variant);
            if (VALUE_RETRIEVED == RetVal)
            {
                pValue->SetDataType(CurrentSchemaType);
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
        uint8_t LengthSize = 0;
        //
        // PRECONDITIONS
        //
        if (pData->IsAtEnd())
        {
            return false;
        }
        if (pData->Peek<uint8_t>() & 0x80)
        {
            LengthSize = pData->Get<uint8_t>() & 0x7F;
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
        
        *pLength = 0;
        while (LengthSize > 0)
        {
            if (pData->IsAtEnd())
            {
                return false;
            }
            
            *pLength <<= 8;
            *pLength |= pData->Get<uint8_t>();

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
    void ASNType::SetDataType(ASN::DataTypes DT)
    {
        m_pSchema = m_SingleDataType;
        m_SingleDataType[0].m_SchemaType = DT;
        Clear();
    }
    
    bool ASNType::GetNextSchemaEntry(ASN::SchemaEntryPtr * ppSchemaEntry)
    {
        *ppSchemaEntry = m_pCurrentSchema;
        if (nullptr == m_pCurrentSchema)
        {
            return false;
        }
        if (ASN::END_SCHEMA_T != m_pCurrentSchema->m_SchemaType)
        {
            ++m_pCurrentSchema;
            return true;
        }
        return false;
    }
    
    bool ASNType::InternalAppend(const DLMSVariant& Value)
    {
        bool                Continue = true;
        uint8_t             ChoiceIndex = 0;
        ASN::SchemaEntryPtr SchemaEntry;
       
        while (GetNextSchemaEntry(&SchemaEntry))
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
        
        while (GetNextSchemaEntry(&SchemaEntry))
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
    ASNObjectIdentifier::ASNObjectIdentifier(ArcList List, OIDType OT /* = ABSOLUTE */)
        : ASNType(ASN::OBJECT_IDENTIFIER)
    {
        bool                Error = false;
        uint8_t             Buffer[sizeof(uintmax_t) + 2];
        uint8_t *           pEnd = &Buffer[sizeof(Buffer)];
        uint8_t *           pStart = pEnd;
        ArcList::iterator   it = List.begin();
        //
        // Tag
        //
        m_Data.Append<uint8_t>(OT);
        //
        // Reserve for length (no long encoding needed for OID)
        //
        m_Data.Append<uint8_t>(0);
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
        m_Data[1] = uint8_t(m_Data.Size() - 2);
    }

    ASNObjectIdentifier::~ASNObjectIdentifier()
    {
    }
    
    bool ASNObjectIdentifier::Peek(const ASNType& Value, DLMSVariant * pVariant, size_t * pBytes /* = nullptr*/)
    {
        DLMSVector Output;
        int        Byte = Value.m_Data.PeekByte();

        if (ABSOLUTE == Byte || RELATIVE == Byte)
        { 
            Output.Append<uint8_t>(Byte);
            Byte = Value.m_Data.PeekByte(1);
            if (Byte >= 0)
            {
                Output.Append<uint8_t>(Byte);
                if (Output.Append(Value.m_Data, 
                    Value.m_Data.GetReadPosition() + 2,
                    Byte) >= 0)
                {
                    pVariant->set<DLMSVector>(Output);
                    if (pBytes)
                    {
                        *pBytes = Output.Size();
                    }
                    return true;
                }
            }
        }
        return false;
    }
    
    bool ASNObjectIdentifier::Get(ASNType * pValue, DLMSVariant * pVariant)
    {
        size_t Bytes = 0;
        if (Peek(*pValue, pVariant, &Bytes))
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
        
    ASNBitString::ASNBitString(size_t BitsExpected, const ASNBitSet& Value) :
        ASNType(ASN::BIT_STRING)
    {
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
        
    bool ASNBitString::Get(ASNBitSet * pBitSet)
    {
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