#pragma once

#include <functional>
#include <map>
#include "Callback.h"
#include "Transport.h"
#include "StateMachine.h"
#include "APDU/ASNType.h"

namespace EPRI
{
    enum COSEMRunResult : uint16_t
    {
        COSEM_RUN_WAIT
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
    
    class COSEM : public Callback<bool, uint16_t>, public StateMachine
    {
    public:
        static const ASNObjectIdentifier ContextLNRNoCipher;
        static const ASNObjectIdentifier ContextSNRNoCipher;
        static const ASNObjectIdentifier MechanismNameLowLevelSecurity;
        static const ASNObjectIdentifier MechanismNameHighLevelSecurity;
        
        enum SecurityLevel : uint8_t
        {
            SECURITY_NONE       = 0,
            SECURITY_LOW_LEVEL  = 1,
            SECURITY_HIGH_LEVEL = 2
        };
        
        typedef int TRANSPORT_HANDLE;
        
        COSEM();
        virtual ~COSEM();
    	
        virtual COSEMRunResult Process();
        virtual TRANSPORT_HANDLE RegisterTransport(Transport * pTransport);

    protected:
        //
        // State Machine
        //
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
        virtual size_t MaxTransports();
        
        std::map<TRANSPORT_HANDLE, Transport *> m_Transports;
        
    };
    //
    // OPEN
    //
    struct APPOpenConfirmOrResponse
    {
        static const uint16_t ID = 0x2001;
        APPOpenConfirmOrResponse(bool LogicalNameReferencing = true, bool WithCiphering = false)
            : m_LogicalNameReferencing(LogicalNameReferencing)
            , m_WithCiphering(WithCiphering)
        {
        }
        // Application Context Name Building
        //
        bool m_LogicalNameReferencing;
        bool m_WithCiphering;
    };
    
    struct APPOpenRequestOrIndication
    {
        static const uint16_t ID = 0x2002;
        APPOpenRequestOrIndication(COSEM::SecurityLevel Security = COSEM::SECURITY_NONE, std::string Password = "", 
                                   bool LogicalNameReferencing = true, bool WithCiphering = false) : 
            m_LogicalNameReferencing(LogicalNameReferencing), 
            m_WithCiphering(WithCiphering),
            m_SecurityLevel(Security),
            m_Password(Password)
        {
        }
        // Application Context Name Building
        //
        bool                 m_LogicalNameReferencing;
        bool                 m_WithCiphering;
        //
        // Mechanism and Security
        //
        COSEM::SecurityLevel m_SecurityLevel;
        std::string          m_Password;
    };

    using ConnectRequestEventData = COSEMEventData<APPOpenRequestOrIndication>;
    
    class COSEMClient : public COSEM
    {
    public:
        COSEMClient();
        virtual ~COSEMClient();
        //
        // COSEM-OPEN Service
        //
        bool OpenRequest(const APPOpenRequestOrIndication& Parameters);
        void RegisterOpenConfirm(CallbackFunction Callback);

    protected:
        //
        // State Machine
        //
        void ST_Inactive_Handler(EventData * pData);
        void ST_Idle_Handler(EventData * pData);
        void ST_Association_Pending_Handler(EventData * pData);
        void ST_Association_Release_Pending_Handler(EventData * pData);
        void ST_Associated_Handler(EventData * pData);
        //
        // Helpers
        //
        Transport * GetTransport() const;

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
        bool ConnectResponse(const APPOpenConfirmOrResponse& Parameters);

    protected:
        void ST_Inactive_Handler(EventData * pData);
        void ST_Idle_Handler(EventData * pData);
        void ST_Association_Pending_Handler(EventData * pData);
        void ST_Association_Release_Pending_Handler(EventData * pData);
        void ST_Associated_Handler(EventData * pData);
       
    };
    
}
