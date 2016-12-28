#include "COSEM.h"
#include "APDU/APDUDefs.h"

namespace EPRI
{
    //
    // APPOpenRequestOrIndication
    //
    APPOpenRequestOrIndication::APPOpenRequestOrIndication(AARQ * pAARQ) :
        APPBaseCallbackParameter(pAARQ->GetSourceAddress(), pAARQ->GetDestinationAddress())
    {
        ASNType::GetNextResult RetVal;
        //
        // Application Context is Required
        //
        RetVal = pAARQ->application_context_name.GetNextValue(&m_SecurityOptions.ApplicationContextName);
        if (RetVal != ASNType::GetNextResult::VALUE_RETRIEVED)
        {
            throw std::logic_error("application_context_name Required");
        }
        //
        // Mechanism is Optional
        //
        RetVal = pAARQ->mechanism_name.GetNextValue(&m_SecurityOptions.MechanismName);
        if (RetVal != ASNType::GetNextResult::VALUE_RETRIEVED &&
            RetVal != ASNType::GetNextResult::VALUE_EMPTY)
        {
            throw std::logic_error("mechanism_name Invalid");
        }
        //
        // Sender ACSE Requirements is Optional
        //
        DLMSValue RequestValue;
        RetVal = pAARQ->sender_acse_requirements.GetNextValue(&RequestValue);
        if (RetVal != ASNType::GetNextResult::VALUE_RETRIEVED &&
            RetVal != ASNType::GetNextResult::VALUE_EMPTY)
        {
            throw std::logic_error("sender_acse_requirements Invalid");
        }
        else if (ASNType::GetNextResult::VALUE_RETRIEVED == RetVal &&
                 DLMSValueGet<DLMSBitSet>(RequestValue)[0])
        {
            RetVal = pAARQ->calling_authentication_value.GetNextValue(&m_SecurityOptions.AuthenticationValue);
            if (RetVal != ASNType::GetNextResult::VALUE_RETRIEVED)
            {
                throw std::logic_error("Authentication is Required");
            }
        }
        //
        // xDLMS Context is Optional
        //
        RetVal = pAARQ->user_information.GetNextValue(&RequestValue);
        if (RetVal != ASNType::GetNextResult::VALUE_RETRIEVED &&
            RetVal != ASNType::GetNextResult::VALUE_EMPTY)
        {
            throw std::logic_error("user_information Invalid");
        }
        else if (ASNType::GetNextResult::VALUE_RETRIEVED == RetVal)
        {
            if (!m_xDLMS.Parse(&DLMSValueGet<DLMSVector>(RequestValue)))
            {
                throw std::logic_error("xDLMS Context Invalid");
            }
        }
    } 
    
    bool APPOpenRequestOrIndication::ToAPDU(AARQ * pAARQ)
    {
        //
        // PRECONDITIONS
        //
        // ApplicationContext is the only required parameter
        //
        if (m_SecurityOptions.ApplicationContextName.IsEmpty())
        {
            return false;
        }
        bool RetVal = pAARQ->application_context_name.Append(m_SecurityOptions.ApplicationContextName) &&
                      pAARQ->mechanism_name.Append(m_SecurityOptions.MechanismName);
        if (m_SecurityOptions.Authentication())
        {
            RetVal = RetVal &&
                pAARQ->sender_acse_requirements.Append(ASNBitString(1, m_SecurityOptions.Authentication() ? 1 : 0)) &&
                pAARQ->calling_authentication_value.SelectChoice(m_SecurityOptions.AuthenticationType()) &&
                pAARQ->calling_authentication_value.Append(m_SecurityOptions.AuthenticationValue);
        }
        if (m_xDLMS.Initialized())
        {
            RetVal = RetVal &&
                pAARQ->user_information.Append(m_xDLMS.GetBytes());    
        }
        return RetVal;
    }
    //
    // APPOpenConfirmOrResponse
    //
    APPOpenConfirmOrResponse::APPOpenConfirmOrResponse(AARE * pAARE)
        : APPBaseCallbackParameter(pAARE->GetSourceAddress(), pAARE->GetDestinationAddress())
    {
        ASNType::GetNextResult RetVal;
        //
        // Application Context is Required
        //
        RetVal = pAARE->application_context_name.GetNextValue(&m_SecurityOptions.ApplicationContextName);
        if (RetVal != ASNType::GetNextResult::VALUE_RETRIEVED)
        {
            throw std::logic_error("application_context_name Required");
        }
        //
        // Mechanism is Optional
        //
        RetVal = pAARE->mechanism_name.GetNextValue(&m_SecurityOptions.MechanismName);
        if (RetVal != ASNType::GetNextResult::VALUE_RETRIEVED &&
            RetVal != ASNType::GetNextResult::VALUE_EMPTY)
        {
            throw std::logic_error("mechanism_name Invalid");
        }
        //
        // Sender ACSE Requirements is Optional
        //
        DLMSValue RequestValue;
        RetVal = pAARE->responder_acse_requirements.GetNextValue(&RequestValue);
        if (RetVal != ASNType::GetNextResult::VALUE_RETRIEVED &&
            RetVal != ASNType::GetNextResult::VALUE_EMPTY)
        {
            throw std::logic_error("responder_acse_requirements Invalid");
        }
        else if (ASNType::GetNextResult::VALUE_RETRIEVED == RetVal &&
                 DLMSValueGet<DLMSBitSet>(RequestValue)[0])
        {
            RetVal = pAARE->responding_authentication_value.GetNextValue(&m_SecurityOptions.AuthenticationValue);
            if (RetVal != ASNType::GetNextResult::VALUE_RETRIEVED)
            {
                throw std::logic_error("Authentication is Required");
            }
        }
        //
        // xDLMS Context is Optional
        //
        RetVal = pAARE->user_information.GetNextValue(&RequestValue);
        if (RetVal != ASNType::GetNextResult::VALUE_RETRIEVED &&
            RetVal != ASNType::GetNextResult::VALUE_EMPTY)
        {
            throw std::logic_error("user_information Invalid");
        }
        else if (ASNType::GetNextResult::VALUE_RETRIEVED == RetVal)
        {
            if (!m_xDLMS.Parse(&DLMSValueGet<DLMSVector>(RequestValue)))
            {
                throw std::logic_error("xDLMS Context Invalid");
            }
        }        
        //
        // Result and Diagnostics are Required
        //
        if (ASNType::GetNextResult::VALUE_RETRIEVED == pAARE->result.GetNextValue(&RequestValue) &&
            IsVariant(RequestValue))
        {
            m_Result = (AssociationResultType) DLMSValueGet<int8_t>(RequestValue);
        }
        if (ASNType::GetNextResult::VALUE_RETRIEVED == pAARE->result_source_diagnostic.GetNextValue(&RequestValue) &&
            IsVariant(RequestValue) && pAARE->result_source_diagnostic.GetChoice((int8_t *) &m_DiagnosticSource))
        {
            m_Diagnostic = DLMSValueGet<int8_t>(RequestValue);
        }
    }
    
    bool APPOpenConfirmOrResponse::ToAPDU(AARE * pAARE)
    {
        //
        // PRECONDITIONS
        //
        if (m_SecurityOptions.ApplicationContextName.IsEmpty())
        {
            return false;
        }
        bool RetVal = pAARE->application_context_name.Append(m_SecurityOptions.ApplicationContextName) &&
            pAARE->result.Append(int8_t(m_Result)) &&
            pAARE->result_source_diagnostic.SelectChoice(m_DiagnosticSource) &&
            pAARE->result_source_diagnostic.Append(m_Diagnostic);
        if (m_xDLMS.Initialized())
        {
            RetVal = RetVal &&
                pAARE->user_information.Append(m_xDLMS.GetBytes());    
        }
        return RetVal;
    }
    //
    // APPReleaseRequestOrIndication
    //
    APPReleaseRequestOrIndication::APPReleaseRequestOrIndication(RLRQ * pRLRQ)
        : APPBaseCallbackParameter(pRLRQ->GetSourceAddress(), pRLRQ->GetDestinationAddress())
    {
        ASNType::GetNextResult RetVal;
        DLMSValue              RequestValue;
        //
        // Release Request Reason is Optional
        //
        RetVal = pRLRQ->reason.GetNextValue(&RequestValue);
        if (RetVal != ASNType::GetNextResult::VALUE_RETRIEVED &&
            RetVal != ASNType::GetNextResult::VALUE_EMPTY)
        {
            throw std::logic_error("reason Invalid");
        }
        else if (ASNType::GetNextResult::VALUE_RETRIEVED == RetVal)
        {
            m_Reason = ReleaseReason(DLMSValueGet<int8_t>(RequestValue));
        }
        //
        // xDLMS Context is Optional
        //
        RetVal = pRLRQ->user_information.GetNextValue(&RequestValue);
        if (RetVal != ASNType::GetNextResult::VALUE_RETRIEVED &&
            RetVal != ASNType::GetNextResult::VALUE_EMPTY)
        {
            throw std::logic_error("user_information Invalid");
        }
        else if (ASNType::GetNextResult::VALUE_RETRIEVED == RetVal)
        {
            if (!m_xDLMS.Parse(&DLMSValueGet<DLMSVector>(RequestValue)))
            {
                throw std::logic_error("xDLMS Context Invalid");
            }
        }        
    }
    
    bool APPReleaseRequestOrIndication::ToAPDU(RLRQ * pRLRQ)
    {
        bool RetVal = true;
        if (m_Reason)
        {
            RetVal = pRLRQ->reason.Append((int8_t)m_Reason.value());
        }
        if (m_xDLMS.Initialized())
        {
            RetVal = RetVal &&
                pRLRQ->user_information.Append(m_xDLMS.GetBytes());    
        }
        return RetVal;
    }
    //
    // APPReleaseConfirmOrResponse
    //
    APPReleaseConfirmOrResponse::APPReleaseConfirmOrResponse(RLRE * pRLRE)
        : APPBaseCallbackParameter(pRLRE->GetSourceAddress(), pRLRE->GetDestinationAddress())
    {
        ASNType::GetNextResult RetVal;
        DLMSValue              ResponseValue;
        //
        // Release Request Reason is Required
        //
        RetVal = pRLRE->reason.GetNextValue(&ResponseValue);
        if (RetVal != ASNType::GetNextResult::VALUE_RETRIEVED &&
            RetVal != ASNType::GetNextResult::VALUE_EMPTY)
        {
            throw std::logic_error("reason Invalid");
        }
        else if (ASNType::GetNextResult::VALUE_RETRIEVED == RetVal)
        {
            m_Reason = ReleaseReason(DLMSValueGet<int8_t>(ResponseValue));
        }
        //
        // xDLMS Context is Optional
        //
        RetVal = pRLRE->user_information.GetNextValue(&ResponseValue);
        if (RetVal != ASNType::GetNextResult::VALUE_RETRIEVED &&
            RetVal != ASNType::GetNextResult::VALUE_EMPTY)
        {
            throw std::logic_error("user_information Invalid");
        }
        else if (ASNType::GetNextResult::VALUE_RETRIEVED == RetVal)
        {
            if (!m_xDLMS.Parse(&DLMSValueGet<DLMSVector>(ResponseValue)))
            {
                throw std::logic_error("xDLMS Context Invalid");
            }
        }        
    }
    
    bool APPReleaseConfirmOrResponse::ToAPDU(RLRE * pRLRE)
    {
        bool RetVal = true;
        if (m_Reason)
        {
            RetVal = pRLRE->reason.Append(int8_t(m_Reason.value()));
        }
        if (m_xDLMS.Initialized())
        {
            RetVal = RetVal &&
                pRLRE->user_information.Append(m_xDLMS.GetBytes());    
        }
        return RetVal;
    }
    //
    // COSEM
    //
    COSEM::COSEM(COSEMAddressType Address) :
        StateMachine(ST_MAX_STATES),
        m_Address(Address)
    {
        //
        // State Machine
        //
        BEGIN_STATE_MAP
            STATE_MAP_ENTRY(ST_INACTIVE, COSEM::ST_Inactive_Handler)
            STATE_MAP_ENTRY(ST_IDLE, COSEM::ST_Idle_Handler)
            STATE_MAP_ENTRY(ST_ASSOCIATION_PENDING, COSEM::ST_Association_Pending_Handler)
            STATE_MAP_ENTRY(ST_ASSOCIATION_RELEASE_PENDING, COSEM::ST_Association_Release_Pending_Handler)
            STATE_MAP_ENTRY(ST_ASSOCIATED, COSEM::ST_Associated_Handler)
        END_STATE_MAP
    }
    
    COSEM::~COSEM()
    {
    }
    	
    COSEM::TRANSPORT_HANDLE COSEM::RegisterTransport(Transport * pXPort)
    {
        static TRANSPORT_HANDLE HANDLE_COUNTER = 0;
        if (m_Transports.size() == MaxTransports())
        {
            return -1;
        }
        ++HANDLE_COUNTER;
        m_Transports[HANDLE_COUNTER] = pXPort;
        
        pXPort->RegisterTransportEventHandler(m_Address,
            std::bind(&COSEM::TransportEventHandler, this, std::placeholders::_1));
        //
        // APDU Handler Registration
        //
        pXPort->RegisterAPDUHandler(m_Address, AARQ::Tag,
            std::bind(&COSEM::AARQ_Handler, this, std::placeholders::_1));
        pXPort->RegisterAPDUHandler(m_Address, AARE::Tag,
            std::bind(&COSEM::AARE_Handler, this, std::placeholders::_1));
        pXPort->RegisterAPDUHandler(m_Address, Get_Request_Base::Tag,
            std::bind(&COSEM::GET_Request_Handler, this, std::placeholders::_1));
        pXPort->RegisterAPDUHandler(m_Address, Get_Response_Base::Tag,
            std::bind(&COSEM::GET_Response_Handler, this, std::placeholders::_1));
        pXPort->RegisterAPDUHandler(m_Address, Set_Request_Base::Tag,
            std::bind(&COSEM::SET_Request_Handler, this, std::placeholders::_1));
        pXPort->RegisterAPDUHandler(m_Address, Set_Response_Base::Tag,
            std::bind(&COSEM::SET_Response_Handler, this, std::placeholders::_1));
        pXPort->RegisterAPDUHandler(m_Address, Action_Request_Base::Tag,
            std::bind(&COSEM::ACTION_Request_Handler, this, std::placeholders::_1));
        pXPort->RegisterAPDUHandler(m_Address, Action_Response_Base::Tag,
            std::bind(&COSEM::ACTION_Response_Handler, this, std::placeholders::_1));
        pXPort->RegisterAPDUHandler(m_Address, RLRQ::Tag,
            std::bind(&COSEM::RLRQ_Handler, this, std::placeholders::_1));
        pXPort->RegisterAPDUHandler(m_Address, RLRE::Tag,
            std::bind(&COSEM::RLRE_Handler, this, std::placeholders::_1));
        
        return HANDLE_COUNTER;
    }
    
    bool COSEM::IsOpen() const
    {
        return m_CurrentState == ST_ASSOCIATED;
    }
    
    COSEMAddressType COSEM::GetAddress() const
    {
        return m_Address;
    }
    
    COSEMAddressType COSEM::GetAssociatedAddress() const
    {
        return m_AssociatedAddress;
    }

    size_t COSEM::MaxTransports()
    {
        return 1;
    }
    
    bool COSEM::TransportEventHandler(const Transport::TransportEvent& Event)
    {
        if (ST_INACTIVE == m_CurrentState && Transport::TRANSPORT_CONNECTED == Event)
        {
            return ExternalEvent(ST_IDLE, new TransportEventData(Event));
        }
        else if (Transport::TRANSPORT_DISCONNECTED == Event)
        {
            return ExternalEvent(ST_INACTIVE, new TransportEventData(Event));
        }
        return false;
    }
    //
    // COSEM-ABORT Service
    //
    void COSEM::RegisterAbortIndication(CallbackFunction Callback)
    {
        RegisterCallback(APPAbortIndication::ID, Callback);
    }
    //
    // HELPERS
    //
    Transport * COSEM::GetTransport() const
    {
        if (m_Transports.empty())
        {
            return nullptr;
        }
        return m_Transports.begin()->second;
    }

}