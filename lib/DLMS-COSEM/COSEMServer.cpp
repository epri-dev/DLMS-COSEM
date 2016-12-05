#include <algorithm>

#include "COSEM.h"

namespace EPRI
{
    COSEMServer::COSEMServer(COSEMAddressType SAP) :
        COSEM(SAP),
        LogicalDevice(this)
    {
    }
    
    COSEMServer::~COSEMServer()
    {
    }
    //
    // COSEM
    //
    COSEMRunResult COSEMServer::Process()
    {
        return COSEMRunResult::COSEM_RUN_WAIT;
    }
    //
    // COSEM-OPEN Service
    //
    bool COSEMServer::OpenResponse(const APPOpenConfirmOrResponse& Parameters)
    {
        bool bAllowed = false;
        BEGIN_TRANSITION_MAP
            TRANSITION_MAP_ENTRY(ST_INACTIVE, ST_IGNORED)
            TRANSITION_MAP_ENTRY(ST_IDLE, ST_ASSOCIATION_PENDING)
            TRANSITION_MAP_ENTRY(ST_ASSOCIATION_PENDING, ST_IGNORED)
            TRANSITION_MAP_ENTRY(ST_ASSOCIATION_RELEASE_PENDING, ST_IGNORED)
            TRANSITION_MAP_ENTRY(ST_ASSOCIATED, ST_IGNORED)
        END_TRANSITION_MAP(bAllowed, new OpenResponseEventData(Parameters));
        return bAllowed;
    }
    
    bool COSEMServer::OnOpenIndication(const APPOpenRequestOrIndication& Parameters)
    {
        //
        // Default Handler Does Nothing
        //
        return true;
    }
    //
    // COSEM-GET Service
    //
    bool COSEMServer::GetResponse(const APPGetConfirmOrResponse& Parameters)
    {
        bool bAllowed = false;
        BEGIN_TRANSITION_MAP
            TRANSITION_MAP_ENTRY(ST_INACTIVE, ST_IGNORED)
            TRANSITION_MAP_ENTRY(ST_IDLE, ST_IGNORED)
            TRANSITION_MAP_ENTRY(ST_ASSOCIATION_PENDING, ST_IGNORED)
            TRANSITION_MAP_ENTRY(ST_ASSOCIATION_RELEASE_PENDING, ST_IGNORED)
            TRANSITION_MAP_ENTRY(ST_ASSOCIATED, ST_ASSOCIATED)
        END_TRANSITION_MAP(bAllowed, new GetResponseEventData(Parameters));
        return bAllowed;
    }
    
    bool COSEMServer::OnGetIndication(const APPGetRequestOrIndication& Parameters)
    {
        //
        // Default Handler Does Nothing
        //
        return true;
    }
    //
    // COSEM-RELEASE Service
    //
    bool COSEMServer::ReleaseResponse(const APPReleaseConfirmOrResponse& Parameters)
    {
        bool bAllowed = false;
        BEGIN_TRANSITION_MAP
            TRANSITION_MAP_ENTRY(ST_INACTIVE, ST_IGNORED)
            TRANSITION_MAP_ENTRY(ST_IDLE, ST_IGNORED)
            TRANSITION_MAP_ENTRY(ST_ASSOCIATION_PENDING, ST_IGNORED)
            TRANSITION_MAP_ENTRY(ST_ASSOCIATION_RELEASE_PENDING, ST_ASSOCIATION_RELEASE_PENDING)
            TRANSITION_MAP_ENTRY(ST_ASSOCIATED, ST_IGNORED)
        END_TRANSITION_MAP(bAllowed, new ReleaseResponseEventData(Parameters));
        return bAllowed;
    }

    bool COSEMServer::OnReleaseIndication(const APPReleaseRequestOrIndication& Parameters)
    {
        //
        // Default Handler Does Nothing
        //
        return true;
    }
    //
    // COSEM-ABORT Service
    //
    bool COSEMServer::OnAbortIndication(const APPAbortIndication& Parameters)
    {
        //
        // Default Handler Does Nothing
        //
        return true;
    }
    //
	// State Machine Handlers
    //
    void COSEMServer::ST_Inactive_Handler(EventData * pData)
    {
        TransportEventData * pTransportData = dynamic_cast<TransportEventData *>(pData);
        if (pTransportData && pTransportData->Data == Transport::TRANSPORT_DISCONNECTED)
        {
            OnAbortIndication(APPAbortIndication(m_AssociatedAddress, GetAddress()));
        }
        m_AssociatedAddress = INVALID_ADDRESS;
    }
    
    void COSEMServer::ST_Idle_Handler(EventData * pData)
    {
        m_AssociatedAddress = INVALID_ADDRESS;
    }
    
    void COSEMServer::ST_Association_Pending_Handler(EventData * pData)
    {
        OpenRequestEventData * pEventData;
        if ((pEventData = dynamic_cast<OpenRequestEventData *>(pData)) != nullptr)
        {
            AARE                        Response;
            APPOpenRequestOrIndication& Parameters = pEventData->Data;
            if (OnOpenIndication(Parameters))
            {
                if (Parameters.m_LogicalNameReferencing && !Parameters.m_WithCiphering)
                {
                    Response.application_context_name.Append(ContextLNRNoCipher);
                }
                else if (!Parameters.m_LogicalNameReferencing && !Parameters.m_WithCiphering)
                {
                    Response.application_context_name.Append(ContextSNRNoCipher);
                }
                //
                // TODO - Actually make this work!
                //
                Response.responder_acse_requirements.Append(ASNBitString(1, 1));
                Response.result.Append(int8_t(AARE::AssociationResult::accepted));
                Response.result_source_diagnostic.SelectChoice(AARE::AssociateDiagnosticChoice::acse_service_user);
                Response.result_source_diagnostic.Append(int8_t(AARE::AssociateDiagnosticUser::user_null));
                Response.user_information.Append(DLMSVector({ 0x08, 0x00, 0x06, 0x5F, 0x1F, 0x04, 
                                                                    0x00, 0x00, 0x38, 0x1F, 0x00, 0x9B, 0x00, 0x07 }));    
            }
            else 
            {
                //
                // TODO - Error Code!
                //
            }

            Transport * pTransport = GetTransport();
            if (nullptr != pTransport &&
                pTransport->DataRequest(Transport::DataRequestParameter(GetAddress(),
                                                                        Parameters.m_SourceAddress,
                                                                        Response.GetBytes())))
            {
                InternalEvent(ST_ASSOCIATED, pData);
            }
            else
            {
                InternalEvent(ST_IDLE);
            }
        }
        
    }
    
    void COSEMServer::ST_Association_Release_Pending_Handler(EventData * pData)
    {
        // 
        // Receive RELEASE Request
        //
        ReleaseRequestEventData * pEventData;
        if ((pEventData = dynamic_cast<ReleaseRequestEventData *>(pData)) != nullptr)
        {
            APPReleaseRequestOrIndication& Parameters = pEventData->Data;
            InitiateRelease(Parameters, 
                OnReleaseIndication(Parameters));
            return;
        }        
        // 
        // Transmit RELEASE Response
        //
        Transport *                pTransport = GetTransport();
        ReleaseResponseEventData * pReleaseResponse = dynamic_cast<ReleaseResponseEventData *>(pData);
        if (pTransport && pReleaseResponse)
        {
            Transport::DataRequestParameter TransportParam;
            RLRE                            Response;
            //
            // TODO - For Real
            //
            Response.reason.Append(int8_t(EPRI::RLRE::ReleaseResponseReason::normal)); 
            Response.user_information.Append(DLMSVector({ 0x08, 0x00, 0x06, 0x5F, 0x1F, 0x04, 0x00,
                                                                0x00, 0x38, 0x1F, 0x00, 0x9B, 0x00, 0x07 }));
            
            TransportParam.SourceAddress = GetAddress();
            TransportParam.DestinationAddress = pReleaseResponse->Data.m_SourceAddress;
            TransportParam.Data = Response.GetBytes();
                
            pTransport->DataRequest(TransportParam);
            
            InternalEvent(ST_IDLE);
            return;
        }              
        
    }
    
    void COSEMServer::ST_Associated_Handler(EventData * pData)
    {
        //
        // OPEN Transition
        //
        OpenRequestEventData * pOpenRequest = dynamic_cast<OpenRequestEventData *>(pData);
        if (pOpenRequest)
        {
            m_AssociatedAddress = pOpenRequest->Data.m_SourceAddress;
            return;
        }
        // 
        // Received GET Request
        //
        GetRequestEventData * pGetRequest = dynamic_cast<GetRequestEventData *>(pData);
        if (pGetRequest)
        {
            InitiateGet(pGetRequest->Data,
                OnGetIndication(pGetRequest->Data));
            return;
        }    
        // 
        // Transmit GET Response
        //
        Transport *            pTransport = GetTransport();
        GetResponseEventData * pGetResponse = dynamic_cast<GetResponseEventData *>(pData);
        if (pTransport && pGetResponse)
        {
            Transport::DataRequestParameter TransportParam;

            switch (pGetResponse->Data.m_Type)
            {
            case APPGetConfirmOrResponse::GetResponseType::get_response_normal:
                {
                    Get_Response_Normal Response;
                    Response.invoke_id_and_priority = pGetResponse->Data.m_InvokeIDAndPriority;
                    Response.result.set<DLMSVector>(pGetResponse->Data.m_Data);
                    TransportParam.SourceAddress = GetAddress();
                    TransportParam.DestinationAddress = pGetResponse->Data.m_SourceAddress;
                    TransportParam.Data = Response.GetBytes();
                }
                break;
                
            case APPGetConfirmOrResponse::GetResponseType::get_response_next:
                throw std::logic_error("get_response_next Not Implemented!");
                
            case APPGetConfirmOrResponse::GetResponseType::get_response_with_list:
                throw std::logic_error("get_response_with_list Not Implemented!");
            }
                
            pTransport->DataRequest(TransportParam);
            
        }        
        
    }
    //
    // APDU Handlers
    //
    bool COSEMServer::AARE_Handler(const IAPDUPtr& pAPDU)
    {
        return false;
    }
    
    bool COSEMServer::AARQ_Handler(const IAPDUPtr& pAPDU)
    {
        AARQ *  pAARQ = dynamic_cast<AARQ *>(pAPDU.get());
        if (pAARQ)
        {
            bool bAllowed = false;
            BEGIN_TRANSITION_MAP
                TRANSITION_MAP_ENTRY(ST_INACTIVE, ST_IGNORED)
                TRANSITION_MAP_ENTRY(ST_IDLE, ST_ASSOCIATION_PENDING)
                TRANSITION_MAP_ENTRY(ST_ASSOCIATION_PENDING, ST_IGNORED)
                TRANSITION_MAP_ENTRY(ST_ASSOCIATION_RELEASE_PENDING, ST_IGNORED)
                TRANSITION_MAP_ENTRY(ST_ASSOCIATED, ST_IGNORED)
            //
            // TODO - Really parse
            //
            END_TRANSITION_MAP(bAllowed, 
                new OpenRequestEventData(APPOpenRequestOrIndication(pAARQ->GetSourceAddress(),
                                                                    pAARQ->GetDestinationAddress())));
            return bAllowed;
        }
        return false;
    }
    
    bool COSEMServer::GET_Request_Handler(const IAPDUPtr& pAPDU)
    {
        GetRequestEventData * pEvent = nullptr;
        Get_Request_Normal *  pNormalRequest = dynamic_cast<Get_Request_Normal *>(pAPDU.get());
        if (pNormalRequest)
        {
            pEvent = new GetRequestEventData(APPGetRequestOrIndication(
                pAPDU->GetSourceAddress(),
                pAPDU->GetDestinationAddress(),
                pNormalRequest->invoke_id_and_priority,
                pNormalRequest->cosem_attribute_descriptor));
        }
        if (pEvent)
        {
            bool bAllowed = false;
            BEGIN_TRANSITION_MAP
                TRANSITION_MAP_ENTRY(ST_INACTIVE, ST_IGNORED)
                TRANSITION_MAP_ENTRY(ST_IDLE, ST_IGNORED)
                TRANSITION_MAP_ENTRY(ST_ASSOCIATION_PENDING, ST_IGNORED)
                TRANSITION_MAP_ENTRY(ST_ASSOCIATION_RELEASE_PENDING, ST_IGNORED)
                TRANSITION_MAP_ENTRY(ST_ASSOCIATED, ST_ASSOCIATED)
            END_TRANSITION_MAP(bAllowed, pEvent);
            return bAllowed;
        }
        return false;
    }
    
    bool COSEMServer::GET_Response_Handler(const IAPDUPtr& pAPDU)
    {
        return false;
    }

    bool COSEMServer::RLRQ_Handler(const IAPDUPtr& pAPDU)
    {
        RLRQ *  pRLRQ = dynamic_cast<RLRQ *>(pAPDU.get());
        if (pRLRQ)
        {
            //
            // TODO - Really implement this...
            //
            bool bAllowed = false;
            BEGIN_TRANSITION_MAP
                TRANSITION_MAP_ENTRY(ST_INACTIVE, ST_IGNORED)
                TRANSITION_MAP_ENTRY(ST_IDLE, ST_IGNORED)
                TRANSITION_MAP_ENTRY(ST_ASSOCIATION_PENDING, ST_IGNORED)
                TRANSITION_MAP_ENTRY(ST_ASSOCIATION_RELEASE_PENDING, ST_IGNORED)
                TRANSITION_MAP_ENTRY(ST_ASSOCIATED, ST_ASSOCIATION_RELEASE_PENDING)
            END_TRANSITION_MAP(bAllowed, 
                new ReleaseRequestEventData(APPReleaseRequestOrIndication(pRLRQ->GetSourceAddress(),
                                                                          pRLRQ->GetDestinationAddress(),
                                                                          true)));
            return bAllowed;
        }
        return false;
    }
    
    bool COSEMServer::RLRE_Handler(const IAPDUPtr& pAPDU)
    {
        return false;
    }

}