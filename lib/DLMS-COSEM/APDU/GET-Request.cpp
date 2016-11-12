#include "APDU/GET-Request.h"

namespace EPRI
{
        
    ASN_BEGIN_SCHEMA(Get_Request::Get_Request_Schema)
        ASN_BEGIN_CHOICE
            ASN_BEGIN_CHOICE_ENTRY_WITH_OPTIONS(Get_Request::Get_Request_Choice::get_request_normal, ASN::IMPLICIT)
        //
        // SEQUENCE????
                ASN_INVOKE_ID_AND_PRIORITY
                ASN_BASE_TYPE(ASN::DT_Unsigned16)
                ASN_FIXED_OCTET_STRING_TYPE(ASN::IMPLICIT, 6)
                ASN_BASE_TYPE(ASN::DT_Integer8)
            ASN_END_CHOICE_ENTRY
        ASN_END_CHOICE
        //
        // TODO
        //
    ASN_END_SCHEMA
        
    Get_Request::Get_Request() :
       Get_Request_Base::APDUSingleType(Get_Request::Get_Request_Schema)
    {

    }
    
    Get_Request::~Get_Request()
    {
    }
    
    bool Get_Request::IsValid() const
    {
        return true;
    }
    //
    // Get_Request_Normal
    //
    Get_Request_Normal::Get_Request_Normal()
    {
    }
    
    Get_Request_Normal::~Get_Request_Normal()
    {
    }
        
    bool Get_Request_Normal::Parse(DLMSVector * pData)
    {
        // Perform the base parse, which just loads
        // the stream.
        //
        if (Get_Request::Parse(pData))
        {
            int8_t       Choice;
            DLMSSequence Sequence;

            m_Type.Rewind();
            if (ASNType::GetNextResult::VALUE_RETRIEVED == m_Type.GetNextValue(&Sequence) &&
                m_Type.GetChoice(&Choice) && 
                (Choice == get_request_normal))   
            {
                try
                {
                    invoke_id_and_priority = Sequence[0].get<uint8_t>();
                    cosem_attribute_descriptor.class_id = Sequence[1].get<uint16_t>();
                    cosem_attribute_descriptor.instance_id = Sequence[2].get<DLMSVector>();
                    cosem_attribute_descriptor.attribute_id = Sequence[3].get<int8_t>();
                }
                catch (const std::exception&)
                {
                    return false;
                }
                return true;
            }
        }
        return false;
    }
    
    std::vector<uint8_t> Get_Request_Normal::GetBytes()
    {
        m_Type.Clear();
        if (m_Type.SelectChoice(get_request_normal))
        {
            m_Type.Append(invoke_id_and_priority);
            m_Type.Append(cosem_attribute_descriptor.class_id);
            m_Type.Append(cosem_attribute_descriptor.instance_id);
            m_Type.Append(cosem_attribute_descriptor.attribute_id);
            return Get_Request::GetBytes();
        }
        return std::vector<uint8_t>();
    }


}