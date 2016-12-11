#include "APDU/ACTION-Response.h"

namespace EPRI
{
        
    ASN_BEGIN_SCHEMA(Action_Response::Action_Response_Schema)
        ASN_BEGIN_CHOICE
            ASN_BEGIN_CHOICE_ENTRY_WITH_OPTIONS(Action_Response::Action_Response_Choice::action_response_normal, ASN::IMPLICIT)
                ASN_BEGIN_SEQUENCE
                    ASN_INVOKE_ID_AND_PRIORITY
                    ASN_ENUM_TYPE
                    ASN_GET_DATA_RESULT(ASN::OPTIONAL)
                ASN_END_SEQUENCE
            ASN_END_CHOICE_ENTRY
        ASN_END_CHOICE
        //
        // TODO
        //
    ASN_END_SCHEMA
        
    Action_Response::Action_Response()
        : Action_Response_Base::APDUSingleType(Action_Response::Action_Response_Schema)
    {

    }
    
    Action_Response::~Action_Response()
    {
    }
    
    bool Action_Response::IsValid() const
    {
        return true;
    }
    //
    // Action_Response_Normal
    //
    Action_Response_Normal::Action_Response_Normal()
    {
    }
    
    Action_Response_Normal::~Action_Response_Normal()
    {
    }
        
    bool Action_Response_Normal::Parse(COSEMAddressType SourceAddress,
        COSEMAddressType DestinationAddress,
        DLMSVector * pData)
    {
        // Perform the base parse, which just loads
        // the stream.
        //
        if (Action_Response::Parse(SourceAddress, DestinationAddress, pData))
        {
            int8_t       Choice;
            DLMSValue    Response;

            m_Type.Rewind();
            if (ASNType::GetNextResult::VALUE_RETRIEVED == m_Type.GetNextValue(&Response) &&
                m_Type.GetChoice(&Choice) && (Choice == action_response_normal))   
            {
                try
                {
                    if (IsSequence(Response))
                    {
                        DLMSSequence& Sequence = DLMSValueGetSequence(Response);
                        invoke_id_and_priority = Sequence[0].get<uint8_t>();
                        single_response.result = (APDUConstants::Action_Result) Sequence[1].get<uint8_t>();
                        if (IsBlank(Sequence[2]))
                        {
                            single_response.return_parameters = DLMSOptionalNone;
                        }
                        else
                        {
                            single_response.return_parameters.value() = Sequence[2].get<DLMSVector>();
                        }
                    }
                    else
                    {
                        return false;
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
    
    std::vector<uint8_t> Action_Response_Normal::GetBytes()
    {
        m_Type.Clear();
        if (m_Type.SelectChoice(action_response_normal))
        {
            m_Type.Append(invoke_id_and_priority);
            m_Type.Append((uint8_t) single_response.result);
            if (single_response.return_parameters)
            {
                m_Type.Append(single_response.return_parameters.value().get<DLMSVector>());
            }
            else
            {
                m_Type.Append();
            }
            return Action_Response::GetBytes();                
        }
        return std::vector<uint8_t>();
    }

}