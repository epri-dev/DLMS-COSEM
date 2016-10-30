#include "COSEM.h"
#include "APDU/AARQ.h"

namespace EPRI
{
    
    COSEMClient::COSEMClient()
    {
    }
    
    COSEMClient::~COSEMClient()
    {
    }
    //
    // COSEM-CONNECT Service
    //
    bool COSEMClient::ConnectRequest(const APPConnectRequestOrIndication& Parameters)
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
    
    void COSEMClient::RegisterConnectConfirm(CallbackFunction Callback)
    {
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
        AARQ Request;
        
        Request.application_context_name.Append(ASNObjectIdentifier({ 2, 16, 756, 5, 8, 1, 1 }));
        Request.sender_acse_requirements.Append(
            ASNBitString(Request.sender_acse_requirements.GetCurrentSchemaTypeSize(), 1));
        Request.mechanism_name.Append(ASNObjectIdentifier({ 2, 16, 756, 5, 8, 2, 1 }));
        Request.calling_authentication_value.Append(ASNType(ASN::GraphicString, std::string("33333333")));
        Request.user_information.Append(
            ASNType(ASN::OCTET_STRING, 
            DLMSVector({ 0x01, 0x00, 0x00, 0x00, 0x06, 0x5F, 0x1F, 0x04, 0x00, 0x00, 0x7E, 0x1F, 0x00, 0x00 })));
        Transport * pTransport = GetTransport();
        if (nullptr != pTransport)
        {
            pTransport->DataRequest(Transport::DataRequestParameter(Request.GetBytes()));
        }
        
    }
    
    void COSEMClient::ST_Association_Release_Pending_Handler(EventData * pData)
    {
    }
    
    void COSEMClient::ST_Associated_Handler(EventData * pData)
    {
    }
    
    Transport * COSEMClient::GetTransport() const
    {
        if (m_Transports.empty())
        {
            return nullptr;
        }
        return m_Transports.begin()->second;
    }

}