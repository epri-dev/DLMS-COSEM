#pragma once

#include <functional>
#include "Callback.h"
#include "Transport.h"
#include "StateMachine.h"

namespace EPRI
{
    enum COSEMRunResult : uint16_t
    {
        COSEM_RUN_WAIT
    };
    //
    // CONNECT
    //
    struct APPConnectConfirmOrResponse
    {
        static const uint16_t ID = __COUNTER__;
        APPConnectConfirmOrResponse(bool LogicalNameReferencing = true, bool WithCiphering = false) :
            m_LogicalNameReferencing(LogicalNameReferencing),
            m_WithCiphering(WithCiphering)
        {
        }
        // Application Context Name Building
        //
        bool m_LogicalNameReferencing;
        bool m_WithCiphering;
    };
    
    struct APPConnectRequestOrIndication
    {
        static const uint16_t ID = __COUNTER__;
        APPConnectRequestOrIndication(bool LogicalNameReferencing = true, bool WithCiphering = false)
            : m_LogicalNameReferencing(LogicalNameReferencing)
            , m_WithCiphering(WithCiphering)
        {
        }
        // Application Context Name Building
        //
        bool m_LogicalNameReferencing;
        bool m_WithCiphering;
    };
    
    template<typename TInternal>
        class COSEMEventData : public EventData
        {
        public:
            COSEMEventData(const TInternal& D)
                : Data(D)
            {
            }

            virtual void Release()
            {
                delete this;
            }
        
            TInternal Data;
        
        };
        
    using TransportEventData = COSEMEventData<Transport::TransportEvent>;
    using ConnectRequestEventData = COSEMEventData<APPConnectRequestOrIndication>;
    
    class COSEM : public Callback<bool, uint16_t>, public StateMachine
    {
    public:
        COSEM();
        virtual ~COSEM();
    	
        virtual COSEMRunResult Process();
        virtual void RegisterTransport(Transport& Transport);

    protected:
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
            
        enum States : uint8_t
        {
            ST_INACTIVE = 0,
            ST_IDLE,
            ST_ASSOCIATION_PENDING,
            ST_ASSOCIATION_RELEASE_PENDING,
            ST_ASSOCIATED,
            ST_MAX_STATES
        };
        
        virtual bool TransportEventHandler(const Transport::TransportEvent& Event);

        virtual void ST_Inactive_Handler(EventData * pData) = 0;
        virtual void ST_Idle_Handler(EventData * pData) = 0;
        virtual void ST_Association_Pending_Handler(EventData * pData) = 0;
        virtual void ST_Association_Release_Pending_Handler(EventData * pData) = 0;
        virtual void ST_Associated_Handler(EventData * pData) = 0;
        
    };
    
    class COSEMClient : public COSEM
    {
    public:
        COSEMClient();
        virtual ~COSEMClient();
        //
        // COSEM-CONNECT Service
        //
        bool ConnectRequest(const APPConnectRequestOrIndication& Parameters);
        void RegisterConnectConfirm(CallbackFunction Callback);

    protected:
        //
        // State Machine
        //
        void ST_Inactive_Handler(EventData * pData);
        void ST_Idle_Handler(EventData * pData);
        void ST_Association_Pending_Handler(EventData * pData);
        void ST_Association_Release_Pending_Handler(EventData * pData);
        void ST_Associated_Handler(EventData * pData);

    };
    
    class COSEMServer : public COSEM
    {
    public:
        COSEMServer();
        virtual ~COSEMServer();
        //
        // COSEM-CONNECT Service
        //
        void RegisterConnectIndication(CallbackFunction Callback);
        bool ConnectResponse(const APPConnectConfirmOrResponse& Parameters);

    protected:
        void ST_Inactive_Handler(EventData * pData);
        void ST_Idle_Handler(EventData * pData);
        void ST_Association_Pending_Handler(EventData * pData);
        void ST_Association_Release_Pending_Handler(EventData * pData);
        void ST_Associated_Handler(EventData * pData);
       
    };
    
}
