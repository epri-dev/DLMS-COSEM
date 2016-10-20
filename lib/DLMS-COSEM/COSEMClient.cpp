#include "COSEM.h"

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
    }
    
    void COSEMClient::ST_Association_Release_Pending_Handler(EventData * pData)
    {
    }
    
    void COSEMClient::ST_Associated_Handler(EventData * pData)
    {
    }
    
}