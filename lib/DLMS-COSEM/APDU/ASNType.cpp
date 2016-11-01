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
    // ASNType
    //
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
        if ((Options & ASN::OPTIONAL) && 
            (0 == Length))
        {
            return std::vector<uint8_t>();
        }
        // Outer tag
        RetVal.Append<uint8_t>(Tag | (Options & ASN::CONSTRUCTED ? 0b00100000 : 0x00));
        // Length   
        if (!(Options & ASN::CONSTRUCTED))
        {
            if (ASN::OBJECT_IDENTIFIER == m_pSchema->m_SchemaType)
            {
                Length -= 2;
                Position += 2;
            }
        }
        // Length
        if (AppendLength(Length, &RetVal))
        {
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
    }
    
    void ASNType::Rewind()
    {
        m_pCurrentSchema = m_pSchema;
        m_AppendState = ST_SIMPLE;
        m_Data.SetReadPosition(0);
    }

    bool ASNType::GetCurrentSchemaValue(DLMSVariant * pVariant) const
    {
        bool RetVal = false;
        //
        // PRECONDITIONS
        //
        if (m_Data.Size() == 0)
        {
            return false;
        }
        
        switch (GetCurrentSchemaType())
        {
        case ASN::INTEGER:
            {
                // Validate our appropriate lengths for this data type.
                //
                const std::set<uint8_t> VALID_LENGTHS({ 1, 2, 4, 8 });
                if (m_Data.PeekByte() == ASN::INTEGER &&
                    VALID_LENGTHS.find(m_Data.PeekByte(1)) != VALID_LENGTHS.end())
                {
                    switch (m_Data.PeekByte(1))
                    {
                    case 1:
                        RetVal = m_Data.Peek<int8_t>(pVariant);
                        break;
                    case 2:
                        RetVal = m_Data.Peek<int16_t>(pVariant);
                        break;
                    case 4:
                        RetVal = m_Data.Peek<int32_t>(pVariant);
                        break;
                    case 8:
                        RetVal = m_Data.Peek<int64_t>(pVariant);
                        break;
                    default:
                        RetVal = false;
                        break;                        
                    }
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
    
    bool ASNType::Append(const ASNType& Value)
    {
        return InternalAppend(Value);
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
    //
    // Operators
    //
    bool ASNType::operator==(const std::vector<uint8_t>& rhs)
    {
        return rhs == m_Data.GetBytes();
    }
    //
    // Protected Methods
    //
    bool ASNType::GetNextSchemaEntry(ASN::SchemaEntryPtr * ppSchemaEntry)
    {
        *ppSchemaEntry = m_pCurrentSchema;
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
                    {
                        if (Value.which() == VAR_STRING)
                        {
                            AppendLength(Value.get<std::string>().length(), &m_Data);
                            m_Data.Append(Value.get<std::string>());
                            return true;
                        }
                        return false;
                    }
                    break;
                case ASN::OCTET_STRING:
                    {
                        if (Value.which() == VAR_VECTOR)
                        {
                            m_Data.Append<uint8_t>(ASN::OCTET_STRING);
                            AppendLength(Value.get<DLMSVector>().Size(), &m_Data);
                            m_Data.Append(Value.get<DLMSVector>());
                            return true;
                        }
                        return false;
                    }
                    break;
                case ASN::INTEGER:
                    {
                        m_Data.Append<uint8_t>(ASN::INTEGER);
                        size_t LengthIndex = m_Data.Append<uint8_t>(0);
                        m_Data.Append(Value);
                        m_Data[LengthIndex] = m_Data.Size() - LengthIndex - 1;
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
    
    bool ASNType::InternalAppend(const ASNType& Value)
    {
        uint8_t             ChoiceIndex = 0;
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
                else if (ASN::INTEGER_LIST_T == 
                    ASN_SCHEMA_INTERNAL_DATA_TYPE(SchemaEntry))
                {
//                    if (ASN::INTEGER == Value.GetCurrentSchemaType() &&
//                        SchemaEntry->m_Extra.which() == VAR_INIT_LIST &&
//                        IsValueInVariant(SchemaEntry->m_Extra, Value.GetCurrentSchemaValue()))
//                    {
//                        InternalAppend(Value.m_Data);
//                        return true;
//                    }
                }
                else if (ASN_SCHEMA_DATA_TYPE(SchemaEntry) ==
                            ASN_SCHEMA_DATA_TYPE(Value.m_pSchema))
                {
                    InternalAppend(Value.m_Data);
                    return true;
                }
                return false;
            case ST_CHOICE:
                if (ASN::END_CHOICE_T == 
                    ASN_SCHEMA_INTERNAL_DATA_TYPE(SchemaEntry))
                {
                    ChoiceIndex = 0;
                    m_AppendState = ST_SIMPLE;  
                    if (!Appended)
                    {
                        return false;
                    }
                    return true;
                }
                if (!Appended &&
                    (ASN_SCHEMA_DATA_TYPE(SchemaEntry) ==
                        Value.m_pSchema->m_SchemaType))
                {
                    m_Data.Append<uint8_t>(0x80 | ChoiceIndex);
                    Appended = InternalAppend(Value.m_Data);
                }
                ++ChoiceIndex;
                break;
            default:
                return false;
            }
        }
        return false;
    }
    
    bool ASNType::InternalAppend(const DLMSVector& Value)
    {
        bool RetVal = true;
        m_Data.Append(Value);
        return RetVal;
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