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
            ASN_OCTET_STRING_TYPE
        ASN_END_SCHEMA
        ASN_BEGIN_SCHEMA(ObjectIdentifierSchema)
            ASN_OBJECT_IDENTIFIER_TYPE
        ASN_END_SCHEMA
        ASN_BEGIN_SCHEMA(IntegerSchema)
            ASN_INTEGER_TYPE
        ASN_END_SCHEMA
        ASN_BEGIN_SCHEMA(GraphicStringSchema)
            ASN_GraphicString_TYPE
        ASN_END_SCHEMA
        
    }
    //
    // ASNType
    //
    ASNType::ASNType(ASN::SchemaType Schema) : 
        m_pSchema(Schema),
        m_pCurrentSchema(Schema)
    {
    }
    
    ASNType::ASNType(ASN::DataTypes DT) :
        m_pSchema(m_SingleDataType),
        m_pCurrentSchema(m_SingleDataType)
    {
        m_SingleDataType[0] = DT;
    }
    
    ASNType::ASNType(ASN::DataTypes DT, const ASNVariant& Value) :
        m_pSchema(m_SingleDataType),
        m_pCurrentSchema(m_SingleDataType)
    {
        m_SingleDataType[0] = DT;
        InternalAppend(Value);
    }

    ASNType::~ASNType()
    {
    }
        
    std::vector<uint8_t> ASNType::GetBytes(ASN::TagIDType Tag, ASN::ComponentOptionType Options)
    {
        std::vector<uint8_t>           RetVal;
        ASNRawDataType::iterator       it = m_Data.begin();
        size_t                         Length = m_Data.size();
        
        // If this is optional or default and not data, then
        // we have nothing to give
        //
        if ((Options & (ASN::OPTIONAL | ASN::DEFAULT)) && 
            (0 == Length))
        {
            return RetVal;
        }
        // Outer tag
        RetVal.push_back(Tag | (Options & ASN::EXPLICIT));
        // Length   
        if (Options & ASN::IMPLICIT)
        {
            if (ASN::OBJECT_IDENTIFIER == *m_pSchema)
            {
                Length -= 2;
                it += 2;
            }
        }
        // Length
        if (AppendLength(Length, &RetVal))
        {
            // Data
            RetVal.insert(RetVal.end(), it, m_Data.end());
        }
        return RetVal;
    }
    
    std::vector<uint8_t> ASNType::GetBytes() const
    {
        return m_Data;
    }
        
    bool ASNType::IsEmpty() const
    {
        return m_Data.empty();
    }

    void ASNType::Clear()
    {
        m_Data.clear();
    }
    
    void ASNType::Rewind()
    {
        m_pCurrentSchema = m_pSchema;
        m_AppendState = SIMPLE;
    }

    bool ASNType::Get(ASNVariant * pValue)
    {
        return false;
    }
    
    bool ASNType::Append(const ASNVariant& Value)
    {
        return InternalAppend(Value);
    }
    
    bool ASNType::Append(const ASNType& Value)
    {
        return InternalAppend(Value);
    }

    bool ASNType::AppendLength(size_t Length, std::vector<uint8_t> * pData)
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
            pData->push_back(0x80 | WorkingLength);
            pData->insert(pData->end(), Buffer + sizeof(Buffer) - WorkingLength, 
                Buffer + sizeof(Buffer));
        }
        else
        {
            pData->push_back(uint8_t(Length));
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
        return rhs == m_Data;
    }
    //
    // Protected Methods
    //
    bool ASNType::GetNextSchemaEntry(ASN::SchemaBaseType * pSchemaEntry)
    {
        *pSchemaEntry = *m_pCurrentSchema;
        if (ASN::END_SCHEMA_T != *m_pCurrentSchema)
        {
            ++m_pCurrentSchema;
            return true;
        }
        return false;
    }
    
    bool ASNType::InternalAppend(const ASNVariant& Value)
    {
        bool                Continue = true;
        uint8_t             ChoiceIndex = 0;
        ASN::SchemaBaseType SchemaEntry;
       
        while (GetNextSchemaEntry(&SchemaEntry))
        {
            switch (m_AppendState)
            {
            case SIMPLE:
                if (ASN::BEGIN_CHOICE_T == 
                    ASN_SCHEMA_INTERNAL_DATA_TYPE(SchemaEntry))
                {
                    m_AppendState = CHOICE;
                    break;
                }
                switch (ASN_SCHEMA_DATA_TYPE(SchemaEntry))
                {
                case ASN::GraphicString:
                    {
                        if (Value.which() == VAR_STRING)
                        {
                            AppendLength(Value.get<std::string>().length(), &m_Data);
                            m_Data.insert(m_Data.end(),
                                Value.get<std::string>().begin(),
                                Value.get<std::string>().end());
                            return true;
                        }
                        return false;
                    }
                    break;
                case ASN::OCTET_STRING:
                    {
                        if (Value.which() == VAR_VECTOR)
                        {
                            m_Data.push_back(ASN::OCTET_STRING);
                            AppendLength(Value.get<std::vector<uint8_t>>().size(), &m_Data);
                            m_Data.insert(m_Data.end(),
                                Value.get<std::vector<uint8_t>>().begin(),
                                Value.get<std::vector<uint8_t>>().end());
                            return true;
                        }
                        return false;
                    }
                    break;
       
                case ASN::VOID:
                default:
                    break;
                }
                break;
            case CHOICE:
                if (ASN::END_CHOICE_T == 
                    ASN_SCHEMA_INTERNAL_DATA_TYPE(SchemaEntry))
                {
                    ChoiceIndex = 0;
                    m_AppendState = SIMPLE;  
                }
                break;
            }
        }
        return false;
    }
    
    bool ASNType::InternalAppend(const ASNType& Value)
    {
        uint8_t             ChoiceIndex = 0;
        ASN::SchemaBaseType SchemaEntry;
        bool                Appended = false;
        
        while (GetNextSchemaEntry(&SchemaEntry))
        {
            switch (m_AppendState)
            {
            case SIMPLE:
                if (ASN::BEGIN_CHOICE_T == 
                    ASN_SCHEMA_INTERNAL_DATA_TYPE(SchemaEntry))
                {
                    m_AppendState = CHOICE;
                    break;
                }
                else if (ASN_SCHEMA_DATA_TYPE(SchemaEntry) ==
                            *Value.m_pSchema)
                {
                    InternalAppend(Value.GetBytes());
                    return true;
                }
                return false;
            case CHOICE:
                if (ASN::END_CHOICE_T == 
                    ASN_SCHEMA_INTERNAL_DATA_TYPE(SchemaEntry))
                {
                    ChoiceIndex = 0;
                    m_AppendState = SIMPLE;  
                    if (!Appended)
                    {
                        return false;
                    }
                    return true;
                }
                if (!Appended &&
                    (ASN_SCHEMA_DATA_TYPE(SchemaEntry) ==
                        *Value.m_pSchema))
                {
                    m_Data.push_back(0x80 | ChoiceIndex);
                    Appended = InternalAppend(Value.GetBytes());
                }
                ++ChoiceIndex;
                break;
            default:
                return false;
            }
        }
        return false;
    }
    
    bool ASNType::InternalAppend(const ASNRawDataType& Value)
    {
        bool RetVal = true;
        m_Data.reserve(m_Data.size() + Value.size());
        m_Data.insert(m_Data.end(), Value.begin(), Value.end());
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
        m_Data.push_back(OT);
        //
        // Reserve for length (no long encoding needed for OID)
        //
        m_Data.push_back(0);
        if (ABSOLUTE == OT && List.size() >= 2)
        {
            uintmax_t FirstArc = *it++;
            m_Data.push_back((40 * FirstArc) + (*it++));
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
                m_Data.clear();
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
                m_Data.push_back(Value);
            }
        }
        m_Data[1] = uint8_t(m_Data.size() - 2);
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
        m_Data.push_back(8 - (BitsExpected % 8));
        for (uint8_t ByteIndex = 0; ByteIndex < ByteOffset; ++ByteIndex)
        {
            uint8_t CurrentByte = 0;
            for (int BitShift = 7; BitShift >= 0; BitShift--)
            {
                CurrentByte |= (Value[ValueBitIndex++] << BitShift);
            }
            m_Data.push_back(CurrentByte);
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