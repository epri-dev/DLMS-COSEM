#include "COSEM.h"

namespace EPRI
{
    COSEM::COSEM() :
        StateMachine(ST_MAX_STATES)
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
    	
    COSEMRunResult COSEM::Process()
    {
        return COSEM_RUN_WAIT;
    }
     
    void COSEM::RegisterTransport(Transport& XPort)
    {
        XPort.RegisterTransportEventHandler(
            std::bind(&COSEM::TransportEventHandler, this, std::placeholders::_1));
    }
    
    bool COSEM::TransportEventHandler(const Transport::TransportEvent& Event)
    {
        bool bAllowed = false;
        BEGIN_TRANSITION_MAP
            TRANSITION_MAP_ENTRY(ST_INACTIVE, ST_IDLE)
            TRANSITION_MAP_ENTRY(ST_IDLE, EVENT_IGNORED)
            TRANSITION_MAP_ENTRY(ST_ASSOCIATION_PENDING, EVENT_IGNORED)
            TRANSITION_MAP_ENTRY(ST_ASSOCIATION_RELEASE_PENDING, EVENT_IGNORED)
            TRANSITION_MAP_ENTRY(ST_ASSOCIATED, ST_INACTIVE)
        END_TRANSITION_MAP(bAllowed, new TransportEventData(Event));
        return bAllowed;
    }

}