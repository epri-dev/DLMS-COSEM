#include "APDU/SET-Request.h"

namespace EPRI
{
        
    ASN_BEGIN_SCHEMA(Set_Request::Set_Request_Schema)
        ASN_BEGIN_CHOICE
            ASN_BEGIN_CHOICE_ENTRY_WITH_OPTIONS(Set_Request::Set_Request_Choice::set_request_normal, ASN::IMPLICIT)
                ASN_BEGIN_SEQUENCE
                    ASN_INVOKE_ID_AND_PRIORITY
                    ASN_BASE_TYPE(ASN::DT_Unsigned16)
                    ASN_FIXED_OCTET_STRING_TYPE(ASN::IMPLICIT, 6)
                    ASN_BASE_TYPE(ASN::DT_Integer8)
                    //
                    // TODO - Selective Access
                    //
                    ASN_BASE_TYPE(ASN::DT_Unsigned8)
                    ASN_DATA_TYPE
                ASN_END_SEQUENCE
            ASN_END_CHOICE_ENTRY
            //
            // TODO - Other Request Types
            //
        ASN_END_CHOICE
    ASN_END_SCHEMA
        
    Set_Request::Set_Request()
        : Set_Request_Base::APDUSingleType(Set_Request::Set_Request_Schema)
    {

    }
    
    Set_Request::~Set_Request()
    {
    }
    
    bool Set_Request::IsValid() const
    {
        return true;
    }
    //
    // Set_Request_Normal
    //
    Set_Request_Normal::Set_Request_Normal()
    {
    }
    
    Set_Request_Normal::~Set_Request_Normal()
    {
    }
        
    bool Set_Request_Normal::Parse(DLMSVector * pData,
        COSEMAddressType SourceAddress,
        COSEMAddressType DestinationAddress)
    {
        // Perform the base parse, which just loads
        // the stream.
        //
        if (Set_Request::Parse(pData, SourceAddress, DestinationAddress))
        {
            int8_t       Choice;
            DLMSValue    Value;

            m_Type.Rewind();
            if (ASNType::GetNextResult::VALUE_RETRIEVED == m_Type.GetNextValue(&Value) &&
                m_Type.GetChoice(&Choice) && 
                (Choice == set_request_normal))   
            {
                try
                {
                    if (IsSequence(Value))
                    {
                        DLMSSequence& Sequence = DLMSValueGetSequence(Value);
                        invoke_id_and_priority = Sequence[0].get<uint8_t>();
                        cosem_attribute_descriptor.class_id = Sequence[1].get<uint16_t>();
                        cosem_attribute_descriptor.instance_id = Sequence[2].get<DLMSVector>();
                        cosem_attribute_descriptor.attribute_id = Sequence[3].get<int8_t>();
                        //
                        // TODO - Selective Access
                        //
                        value = Sequence[5].get<DLMSVector>();
                        return true;
                    }
                }
                catch (const std::exception&)
                {
                }
            }
        }
        return false;
    }
    
    std::vector<uint8_t> Set_Request_Normal::GetBytes()
    {
        m_Type.Clear();
        if (m_Type.SelectChoice(set_request_normal))
        {
            m_Type.Append(invoke_id_and_priority);
            m_Type.Append(cosem_attribute_descriptor.class_id);
            m_Type.Append(cosem_attribute_descriptor.instance_id);
            m_Type.Append(cosem_attribute_descriptor.attribute_id);
            //
            // TODO - Selective Access
            //
            m_Type.Append(uint8_t(0x00));
            m_Type.Append(value);
            return Set_Request::GetBytes();
        }
        return std::vector<uint8_t>();
    }


}