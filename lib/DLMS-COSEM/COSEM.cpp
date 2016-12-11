#include "COSEM.h"
#include "APDU/APDUDefs.h"

namespace EPRI
{
    
    const ASNObjectIdentifier COSEM::ContextLNRNoCipher({ 2, 16, 756, 5, 8, 1, 1 });
    const ASNObjectIdentifier COSEM::ContextSNRNoCipher({ 2, 16, 756, 5, 8, 1, 2 });
    const ASNObjectIdentifier COSEM::MechanismNameLowLevelSecurity({ 2, 16, 756, 5, 8, 2, 1 }, 
                                                                   ASN::IMPLICIT);
    const ASNObjectIdentifier COSEM::MechanismNameHighLevelSecurity({ 2, 16, 756, 5, 8, 2, 5 }, 
                                                                    ASN::IMPLICIT);

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