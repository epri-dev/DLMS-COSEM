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
            TRANSITION_MAP_ENTRY(ST_IDLE, ST_IGNORED)
            TRANSITION_MAP_ENTRY(ST_ASSOCIATION_PENDING, ST_ASSOCIATION_PENDING)
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
    // COSEM-SET Service
    //
    bool COSEMServer::SetResponse(const APPSetConfirmOrResponse& Parameters)
    {
        bool bAllowed = false;
        BEGIN_TRANSITION_MAP
            TRANSITION_MAP_ENTRY(ST_INACTIVE, ST_IGNORED)
            TRANSITION_MAP_ENTRY(ST_IDLE, ST_IGNORED)
            TRANSITION_MAP_ENTRY(ST_ASSOCIATION_PENDING, ST_IGNORED)
            TRANSITION_MAP_ENTRY(ST_ASSOCIATION_RELEASE_PENDING, ST_IGNORED)
            TRANSITION_MAP_ENTRY(ST_ASSOCIATED, ST_ASSOCIATED)
        END_TRANSITION_MAP(bAllowed, new SetResponseEventData(Parameters));
        return bAllowed;
    }
    
    bool COSEMServer::OnSetIndication(const APPSetRequestOrIndication& Parameters)
    {
        //
        // Default Handler Does Nothing
        //
        return true;
    }
    // COSEM-ACTION Service
    //
    bool COSEMServer::ActionResponse(const APPActionConfirmOrResponse& Parameters)
    {
        bool bAllowed = false;
        BEGIN_TRANSITION_MAP
            TRANSITION_MAP_ENTRY(ST_INACTIVE, ST_IGNORED)
            TRANSITION_MAP_ENTRY(ST_IDLE, ST_IGNORED)
            TRANSITION_MAP_ENTRY(ST_ASSOCIATION_PENDING, ST_IGNORED)
            TRANSITION_MAP_ENTRY(ST_ASSOCIATION_RELEASE_PENDING, ST_IGNORED)
            TRANSITION_MAP_ENTRY(ST_ASSOCIATED, ST_ASSOCIATED)
        END_TRANSITION_MAP(bAllowed, new ActionResponseEventData(Parameters));
        return bAllowed;
    }
    
    bool COSEMServer::OnActionIndication(const APPActionRequestOrIndication& Parameters)
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
            //
            // TODO - Phase II.  When there are more than one association.
            //
            OnAbortIndication(APPAbortIndication(GetAssociatedAddress(), GetAddress()));
        }
        ReleaseTransientAssociations();
    }
    
    void COSEMServer::ST_Idle_Handler(EventData * pData)
    {
        ReleaseTransientAssociations();
    }
    
    void COSEMServer::ST_Association_Pending_Handler(EventData * pData)
    {
        // 
        // Receive OPEN Request
        //
        OpenRequestEventData * pOpenRequest = dynamic_cast<OpenRequestEventData *>(pData);
        if (pOpenRequest)
        {
            APPOpenRequestOrIndication& Parameters = pOpenRequest->Data;
            InitiateOpen(Parameters, 
                OnOpenIndication(Parameters));
            return;
        }        
        //
        // Transmit OPEN Response
        //
        Transport *             pTransport = GetTransport();
        OpenResponseEventData * pOpenResponse = dynamic_cast<OpenResponseEventData *>(pData);
        if (pTransport && pOpenResponse)
        {
            Transport::DataRequestParameter TransportParam;
            AARE                            Response;
            bool                            Associated = false;
            APPOpenConfirmOrResponse&       Parameters = pOpenResponse->Data;
            if (Parameters.ToAPDU(&Response))
            {
                TransportParam.SourceAddress = GetAddress();
                TransportParam.DestinationAddress = Parameters.m_DestinationAddress;
                TransportParam.Data = Response.GetBytes();
                Associated = pTransport->DataRequest(TransportParam);
            }
            if (Associated)
            {
                InternalEvent(ST_ASSOCIATED);
            }
            else
            {
                InternalEvent(ST_IDLE);
            }
            return;
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
            bool                            Released = false;
            APPReleaseConfirmOrResponse&    Parameters = pReleaseResponse->Data;
            if (Parameters.ToAPDU(&Response))
            {
                TransportParam.SourceAddress = GetAddress();
                TransportParam.DestinationAddress = Parameters.m_DestinationAddress;
                TransportParam.Data = Response.GetBytes();
                Released = pTransport->DataRequest(TransportParam);
            }
            if (Released)
            {
                InternalEvent(ST_IDLE);
            }
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
                    Response.result = pGetResponse->Data.m_Result;
                    
                    TransportParam.SourceAddress = GetAddress();
                    TransportParam.DestinationAddress = pGetResponse->Data.m_DestinationAddress;
                    TransportParam.Data = Response.GetBytes();
                }
                break;
                
            case APPGetConfirmOrResponse::GetResponseType::get_response_next:
                throw std::logic_error("get_response_next Not Implemented!");
                
            case APPGetConfirmOrResponse::GetResponseType::get_response_with_list:
                throw std::logic_error("get_response_with_list Not Implemented!");
            }
                
            pTransport->DataRequest(TransportParam);
            
            return;
        } 
        // 
        // Received SET Request
        //
        SetRequestEventData * pSetRequest = dynamic_cast<SetRequestEventData *>(pData);
        if (pSetRequest)
        {
            InitiateSet(pSetRequest->Data,
                OnSetIndication(pSetRequest->Data));
            return;
        }    
        // 
        // Transmit SET Response
        //
        SetResponseEventData * pSetResponse = dynamic_cast<SetResponseEventData *>(pData);
        if (pTransport && pSetResponse)
        {
            Transport::DataRequestParameter TransportParam;

            switch (pSetResponse->Data.m_Type)
            {
            case APPSetConfirmOrResponse::SetResponseType::set_response_normal:
                {
                    Set_Response_Normal Response;
                    const APPSetConfirmOrResponse& Parameters = pSetResponse->Data;
                    
                    Response.invoke_id_and_priority = Parameters.m_InvokeIDAndPriority;
                    Response.result = Parameters.m_Result;

                    TransportParam.SourceAddress = GetAddress();
                    TransportParam.DestinationAddress = Parameters.m_DestinationAddress;
                    TransportParam.Data = Response.GetBytes();
                }
                break;
                
            case APPSetConfirmOrResponse::SetResponseType::set_response_with_first_datablock :
                throw std::logic_error("set_response_with_first_datablock Not Implemented!");

            case APPSetConfirmOrResponse::SetResponseType::set_response_with_datablock :
                throw std::logic_error("set_response_with_datablock Not Implemented!");

            case APPSetConfirmOrResponse::SetResponseType::set_response_with_list :
                throw std::logic_error("set_response_with_list Not Implemented!");

            case APPSetConfirmOrResponse::SetResponseType::set_response_with_list_and_first_datablock :
                throw std::logic_error("set_response_with_list_and_first_datablock Not Implemented!");            }
                
            pTransport->DataRequest(TransportParam);
            
            return;
        }         
        // 
        // Received ACTION Request
        //
        ActionRequestEventData * pActionRequest = dynamic_cast<ActionRequestEventData *>(pData);
        if (pActionRequest)
        {
            InitiateAction(pActionRequest->Data,
                OnActionIndication(pActionRequest->Data));
            return;
        }    
        // 
        // Transmit ACTION Response
        //
        ActionResponseEventData * pActionResponse = dynamic_cast<ActionResponseEventData *>(pData);
        if (pTransport && pActionResponse)
        {
            Transport::DataRequestParameter TransportParam;

            switch (pActionResponse->Data.m_Type)
            {
            case APPActionConfirmOrResponse::ActionResponseType::action_response_normal:
                {
                    Action_Response_Normal            Response;
                    const APPActionConfirmOrResponse& Parameters = pActionResponse->Data;
                    
                    Response.invoke_id_and_priority = Parameters.m_InvokeIDAndPriority;
                    Response.single_response.result = Parameters.m_Result;

                    TransportParam.SourceAddress = GetAddress();
                    TransportParam.DestinationAddress = Parameters.m_DestinationAddress;
                    TransportParam.Data = Response.GetBytes();
                }
                break;
                
            default:
                throw std::logic_error("Action Response Not Implemented!");

            }
                
            pTransport->DataRequest(TransportParam);
            
            return;
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
            try
            {
                BEGIN_TRANSITION_MAP
                    TRANSITION_MAP_ENTRY(ST_INACTIVE, ST_IGNORED)
                    TRANSITION_MAP_ENTRY(ST_IDLE, ST_ASSOCIATION_PENDING)
                    TRANSITION_MAP_ENTRY(ST_ASSOCIATION_PENDING, ST_IGNORED)
                    TRANSITION_MAP_ENTRY(ST_ASSOCIATION_RELEASE_PENDING, ST_IGNORED)
                    TRANSITION_MAP_ENTRY(ST_ASSOCIATED, ST_IGNORED)
                END_TRANSITION_MAP(bAllowed, new OpenRequestEventData(APPOpenRequestOrIndication(pAARQ)));
            }
            catch (const std::exception&)
            {
            }
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

    bool COSEMServer::SET_Request_Handler(const IAPDUPtr& pAPDU)
    {
        SetRequestEventData * pEvent = nullptr;
        Set_Request_Normal *  pNormalRequest = dynamic_cast<Set_Request_Normal *>(pAPDU.get());
        if (pNormalRequest)
        {
            pEvent = new SetRequestEventData(APPSetRequestOrIndication(
                pAPDU->GetSourceAddress(),
                pAPDU->GetDestinationAddress(),
                pNormalRequest->invoke_id_and_priority,
                pNormalRequest->cosem_attribute_descriptor,
                pNormalRequest->value));
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
    
    bool COSEMServer::SET_Response_Handler(const IAPDUPtr& pAPDU)
    {
        return false;
    }

    bool COSEMServer::ACTION_Request_Handler(const IAPDUPtr& pAPDU)
    {
        ActionRequestEventData * pEvent = nullptr;
        Action_Request_Normal *  pNormalRequest = dynamic_cast<Action_Request_Normal *>(pAPDU.get());
        if (pNormalRequest)
        {
            pEvent = new ActionRequestEventData(APPActionRequestOrIndication(
                pAPDU->GetSourceAddress(),
                pAPDU->GetDestinationAddress(),
                pNormalRequest->invoke_id_and_priority,
                pNormalRequest->cosem_method_descriptor,
                pNormalRequest->method_invocation_parameters));
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
    
    bool COSEMServer::ACTION_Response_Handler(const IAPDUPtr& pAPDU)
    {
        return false;
    }
    
    bool COSEMServer::RLRQ_Handler(const IAPDUPtr& pAPDU)
    {
        bool    bAllowed = false;
        RLRQ *  pRLRQ = dynamic_cast<RLRQ *>(pAPDU.get());
        if (pRLRQ)
        {
            try
            {
                BEGIN_TRANSITION_MAP
                    TRANSITION_MAP_ENTRY(ST_INACTIVE, ST_IGNORED)
                    TRANSITION_MAP_ENTRY(ST_IDLE, ST_IGNORED)
                    TRANSITION_MAP_ENTRY(ST_ASSOCIATION_PENDING, ST_IGNORED)
                    TRANSITION_MAP_ENTRY(ST_ASSOCIATION_RELEASE_PENDING, ST_IGNORED)
                    TRANSITION_MAP_ENTRY(ST_ASSOCIATED, ST_ASSOCIATION_RELEASE_PENDING)
                END_TRANSITION_MAP(bAllowed, new ReleaseRequestEventData(APPReleaseRequestOrIndication(pRLRQ)));
            }
            catch (std::exception&)
            {
            }
        }
        return bAllowed;
    }
    
    bool COSEMServer::RLRE_Handler(const IAPDUPtr& pAPDU)
    {
        return false;
    }

}