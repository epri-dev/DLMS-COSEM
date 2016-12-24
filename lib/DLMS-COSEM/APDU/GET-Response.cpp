#include "APDU/GET-Response.h"

namespace EPRI
{
        
    ASN_BEGIN_SCHEMA(Get_Response::Get_Response_Schema)
        ASN_BEGIN_CHOICE
            ASN_BEGIN_CHOICE_ENTRY_WITH_OPTIONS(Get_Response::Get_Response_Choice::get_response_normal, ASN::IMPLICIT)
                ASN_INVOKE_ID_AND_PRIORITY
                ASN_GET_DATA_RESULT(ASN::NO_OPTIONS)
            ASN_END_CHOICE_ENTRY
        ASN_END_CHOICE
        //
        // TODO
        //
    ASN_END_SCHEMA
        
    Get_Response::Get_Response()
        : Get_Response_Base::APDUSingleType(Get_Response::Get_Response_Schema)
    {

    }
    
    Get_Response::~Get_Response()
    {
    }
    
    bool Get_Response::IsValid() const
    {
        return true;
    }
    //
    // Get_Response_Normal
    //
    Get_Response_Normal::Get_Response_Normal()
    {
    }
    
    Get_Response_Normal::~Get_Response_Normal()
    {
    }
        
    bool Get_Response_Normal::Parse(DLMSVector * pData,
        COSEMAddressType SourceAddress,
        COSEMAddressType DestinationAddress)
    {
        // Perform the base parse, which just loads
        // the stream.
        //
        if (Get_Response::Parse(pData, SourceAddress, DestinationAddress))
        {
            int8_t       Choice;
            DLMSValue    InvokeIDAndPriority;
            DLMSValue    Data;

            m_Type.Rewind();
            if (ASNType::GetNextResult::VALUE_RETRIEVED == m_Type.GetNextValue(&InvokeIDAndPriority) &&
                m_Type.GetChoice(&Choice) && 
                (Choice == get_response_normal))   
            {
                try
                {
                    invoke_id_and_priority = DLMSValueGet<uint8_t>(InvokeIDAndPriority);
                    if (ASNType::GetNextResult::VALUE_RETRIEVED == m_Type.GetNextValue(&Data) &&
                        m_Type.GetChoice(&Choice))
                    {
                        if (data == Choice)
                        {
                            result = DLMSValueGet<DLMSVector>(Data);
                        }
                        else if (data_access_result == Choice)
                        {
                            result = DLMSValueGet<uint8_t>(Data);
                        }
                        else
                        {
                            return false;
                        }
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
    
    std::vector<uint8_t> Get_Response_Normal::GetBytes()
    {
        m_Type.Clear();
        if (m_Type.SelectChoice(get_response_normal))
        {
            m_Type.Append(invoke_id_and_priority);
            if (m_Type.SelectChoice(result.which()))
            {
                switch (result.which())
                {
                case data:
                    m_Type.Append(result.get<DLMSVector>());
                    break;
                case data_access_result:
                    m_Type.Append((uint8_t) result.get<APDUConstants::Data_Access_Result>());
                    break;
                }
                return Get_Response::GetBytes();                
            }
        }
        return std::vector<uint8_t>();
    }

}