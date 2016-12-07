#include "APDU/SET-Response.h"

namespace EPRI
{
        
    ASN_BEGIN_SCHEMA(Set_Response::Set_Response_Schema)
        ASN_BEGIN_CHOICE
            ASN_BEGIN_CHOICE_ENTRY_WITH_OPTIONS(Set_Response::Set_Response_Choice::set_response_normal, ASN::IMPLICIT)
                ASN_INVOKE_ID_AND_PRIORITY
                ASN_ENUM_TYPE
            ASN_END_CHOICE_ENTRY
        ASN_END_CHOICE
        //
        // TODO
        //
    ASN_END_SCHEMA
        
    Set_Response::Set_Response()
        : Set_Response_Base::APDUSingleType(Set_Response::Set_Response_Schema)
    {

    }
    
    Set_Response::~Set_Response()
    {
    }
    
    bool Set_Response::IsValid() const
    {
        return true;
    }
    //
    // Set_Response_Normal
    //
    Set_Response_Normal::Set_Response_Normal()
    {
    }
    
    Set_Response_Normal::~Set_Response_Normal()
    {
    }
        
    bool Set_Response_Normal::Parse(COSEMAddressType SourceAddress,
        COSEMAddressType DestinationAddress,
        DLMSVector * pData)
    {
        // Perform the base parse, which just loads
        // the stream.
        //
        if (Set_Response::Parse(SourceAddress, DestinationAddress, pData))
        {
            int8_t       Choice;
            DLMSValue    InvokeIDAndPriority;

            m_Type.Rewind();
            if (ASNType::GetNextResult::VALUE_RETRIEVED == m_Type.GetNextValue(&InvokeIDAndPriority) &&
                m_Type.GetChoice(&Choice) && 
                (Choice == set_response_normal))   
            {
                try
                {
                    DLMSValue    Result;

                    invoke_id_and_priority = DLMSValueGet<uint8_t>(InvokeIDAndPriority);
                    if (ASNType::GetNextResult::VALUE_RETRIEVED == m_Type.GetNextValue(&Result))
                    {
                        result = (APDUConstants::Data_Access_Result) DLMSValueGet<uint8_t>(Result);
                    }
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
    
    std::vector<uint8_t> Set_Response_Normal::GetBytes()
    {
        m_Type.Clear();
        if (m_Type.SelectChoice(set_response_normal))
        {
            m_Type.Append(invoke_id_and_priority);
            m_Type.Append((uint8_t) result);
            return Set_Response::GetBytes();                
        }
        return std::vector<uint8_t>();
    }

}