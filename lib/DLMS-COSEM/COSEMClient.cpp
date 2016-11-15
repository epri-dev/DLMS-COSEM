#include "COSEM.h"
#include "APDU/AARQ.h"
#include "APDU/AARE.h"

namespace EPRI
{
    
    COSEMClient::COSEMClient()
    {
    }
    
    COSEMClient::~COSEMClient()
    {
    }
    //
    // COSEM
    //
    COSEMRunResult COSEMClient::Process()
    {

    }
    //
    // COSEM-OPEN Service
    //
    bool COSEMClient::OpenRequest(const APPOpenRequestOrIndication& Parameters)
    {
        bool bAllowed = false;
        BEGIN_TRANSITION_MAP
            TRANSITION_MAP_ENTRY(ST_INACTIVE, EVENT_IGNORED)
            TRANSITION_MAP_ENTRY(ST_IDLE, ST_ASSOCIATION_PENDING)
            TRANSITION_MAP_ENTRY(ST_ASSOCIATION_PENDING, EVENT_IGNORED)
            TRANSITION_MAP_ENTRY(ST_ASSOCIATION_RELEASE_PENDING, EVENT_IGNORED)
            TRANSITION_MAP_ENTRY(ST_ASSOCIATED, EVENT_IGNORED)
        END_TRANSITION_MAP(bAllowed, new ConnectRequestEventData(Parameters));
        return bAllowed;
    }
    
    void COSEMClient::RegisterOpenConfirm(CallbackFunction Callback)
    {
        RegisterCallback(APPOpenConfirmOrResponse::ID, Callback);
    }
	//
	// State Machine Handlers
    //
    void COSEMClient::ST_Inactive_Handler(EventData * pData)
    {
    }
    
    void COSEMClient::ST_Idle_Handler(EventData * pData)
    {
        TransportEventData * pTransport;
        if ((pTransport = dynamic_cast<TransportEventData *>(pData)) != nullptr)
        {
            // If we have disconnected, then we need to go back to the Inactive state.
            //
            if (pTransport->Data == Transport::TRANSPORT_DISCONNECTED)
            {
                InternalEvent(ST_INACTIVE);
            }
        }
    }
    
    void COSEMClient::ST_Association_Pending_Handler(EventData * pData)
    {
        ConnectRequestEventData * pEventData;
        if ((pEventData = dynamic_cast<ConnectRequestEventData *>(pData)) != nullptr)
        {
            AARQ                        Request;
            APPOpenRequestOrIndication& Parameters = pEventData->Data;
            if (Parameters.m_LogicalNameReferencing && !Parameters.m_WithCiphering)
            {
                Request.application_context_name.Append(ContextLNRNoCipher);
            }
            else if (!Parameters.m_LogicalNameReferencing && !Parameters.m_WithCiphering)
            {
                Request.application_context_name.Append(ContextSNRNoCipher);
            }
            
            Request.sender_acse_requirements.Append(ASNBitString(1, 1));
            
            if (COSEM::SECURITY_LOW_LEVEL == Parameters.m_SecurityLevel)
            {
                Request.mechanism_name.Append(MechanismNameLowLevelSecurity);
                Request.calling_authentication_value.SelectChoice(APDUConstants::AuthenticationValueChoice::charstring);
                Request.calling_authentication_value.Append(Parameters.m_Password);
            }
            else if (COSEM::SECURITY_HIGH_LEVEL == Parameters.m_SecurityLevel)
            {
                Request.mechanism_name.Append(MechanismNameHighLevelSecurity);
                //
                // TODO - HLS
                //
            }
            //
            // TODO - xDLMS
            //
            Request.user_information.Append(DLMSVector({ 0x01, 0x00, 0x00, 0x00, 0x06, 0x5F, 
                                                         0x1F, 0x04, 0x00, 0x00, 0x7E, 0x1F, 0x00, 0x00 }));
            Transport * pTransport = GetTransport();
            if (nullptr != pTransport)
            {
                pTransport->DataRequest(Transport::DataRequestParameter(Request.GetBytes()));
            }
        }
        
    }
    
    void COSEMClient::ST_Association_Release_Pending_Handler(EventData * pData)
    {
    }
    
    void COSEMClient::ST_Associated_Handler(EventData * pData)
    {
        bool                       RetVal = false;
        ConnectResponseEventData * pEvent = dynamic_cast<ConnectResponseEventData *>(pData);
        if (!pEvent || !FireCallback(APPOpenConfirmOrResponse::ID, pEvent->Data, &RetVal) || !RetVal)
        {
            // Denied by upper layers.  Go back to ST_IDLE.
            //
            InternalEvent(ST_IDLE);
        }
        printf("ASSOCIATED\n");
    }
    //
    // APDU Handlers
    //
    bool COSEMClient::AARE_Handler(const IAPDUPtr& pAPDU)
    {
        AARE * pAARE = dynamic_cast<AARE *>(pAPDU.get());
        if (pAARE)
        {
            DLMSValue AssociationResult;
            if (ASNType::GetNextResult::VALUE_RETRIEVED == pAARE->result.GetNextValue(&AssociationResult) &&
                DLMSValueGet<int8_t>(AssociationResult) == AARE::AssociationResult::accepted)
            {
                //
                // TODO - Fill Parameter
                //
                ExternalEvent(ST_ASSOCIATED, 
                    new ConnectResponseEventData(APPOpenConfirmOrResponse()));
            }
            else
            {
                InternalEvent(ST_IDLE);
            }
            return true;    
        }
        return false;
    }

    bool COSEMClient::AARQ_Handler(const IAPDUPtr& pAPDU)
    {
        return false;
    }
    
    bool COSEMClient::GET_Request_Handler(const IAPDUPtr& pAPDU)
    {
        return false;
    }
    
    bool COSEMClient::GET_Response_Handler(const IAPDUPtr& pAPDU)
    {
        return false;
    }
    //
    // HELPERS
    //
    Transport * COSEMClient::GetTransport() const
    {
        if (m_Transports.empty())
        {
            return nullptr;
        }
        return m_Transports.begin()->second;
    }

}