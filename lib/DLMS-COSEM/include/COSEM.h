#pragma once

#include <functional>
#include <map>
#include <memory>

#include "interfaces/COSEMAttribute.h"
#include "Callback.h"
#include "Transport.h"
#include "StateMachine.h"
#include "APDU/ASNType.h"
#include "LogicalDevice.h"

namespace EPRI
{
    class IAPDU;
    class LogicalDevice;    
    
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
        
        COSEM() = delete;
        COSEM(COSEMAddressType Address);
        virtual ~COSEM();
    	
        virtual COSEMRunResult Process() = 0;
        virtual TRANSPORT_HANDLE RegisterTransport(Transport * pTransport);
        virtual bool IsOpen() const;
        virtual COSEMAddressType GetAddress() const;

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
        
        virtual size_t MaxTransports();
        virtual bool TransportEventHandler(const Transport::TransportEvent& Event);

        virtual void ST_Inactive_Handler(EventData * pData) = 0;
        virtual void ST_Idle_Handler(EventData * pData) = 0;
        virtual void ST_Association_Pending_Handler(EventData * pData) = 0;
        virtual void ST_Association_Release_Pending_Handler(EventData * pData) = 0;
        virtual void ST_Associated_Handler(EventData * pData) = 0;
        //
        // APDU Handlers
        //
        virtual bool AARQ_Handler(const IAPDUPtr& pAPDU) = 0;
        virtual bool AARE_Handler(const IAPDUPtr& pAPDU) = 0;
        virtual bool GET_Request_Handler(const IAPDUPtr& pAPDU) = 0;
        virtual bool GET_Response_Handler(const IAPDUPtr& pAPDU) = 0;
        //
        // Helpers
        //
        virtual Transport * GetTransport() const;
        
        std::map<TRANSPORT_HANDLE, Transport *> m_Transports;
        COSEMAddressType                        m_Address;
        
    };
    //
    // BaseCallbackParameter for COSEM
    //
    struct APPBaseCallbackParameter : public BaseCallbackParameter
    {
        APPBaseCallbackParameter(COSEMAddressType SourceAddress,
            COSEMAddressType DestinationAddress) :
            m_SourceAddress(SourceAddress), m_DestinationAddress(DestinationAddress)
        {
        }
        COSEMAddressType  m_SourceAddress;
        COSEMAddressType  m_DestinationAddress;
    };
    //
    // OPEN Service
    //
    struct APPOpenConfirmOrResponse : public APPBaseCallbackParameter
    {
        static const uint16_t ID = 0x2001;
        APPOpenConfirmOrResponse(COSEMAddressType SourceAddress,
                                 COSEMAddressType DestinationAddress,
                                 bool LogicalNameReferencing = true, 
                                 bool WithCiphering = false) : 
            APPBaseCallbackParameter(SourceAddress, DestinationAddress),
            m_LogicalNameReferencing(LogicalNameReferencing), m_WithCiphering(WithCiphering)
        {
        }
        // Application Context Name Building
        //
        bool m_LogicalNameReferencing;
        bool m_WithCiphering;
    };
    
    struct APPOpenRequestOrIndication : public APPBaseCallbackParameter
    {
        static const uint16_t ID = 0x2002;
        APPOpenRequestOrIndication(COSEMAddressType SourceAddress,
                                   COSEMAddressType DestinationAddress,
                                   COSEM::SecurityLevel Security = COSEM::SECURITY_NONE, 
                                   std::string Password = "", 
                                   bool LogicalNameReferencing = true, bool WithCiphering = false) : 
            APPBaseCallbackParameter(SourceAddress, DestinationAddress),
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

    using OpenRequestEventData = COSEMEventData<APPOpenRequestOrIndication>;
    using OpenResponseEventData = COSEMEventData<APPOpenConfirmOrResponse>;
    //
    // GET Service
    //
    struct APPGetRequestOrIndication : public APPBaseCallbackParameter
    {
        static const uint16_t ID = 0x2003;
        typedef variant<Cosem_Attribute_Descriptor, uint32_t> RequestParameter;

        APPGetRequestOrIndication(COSEMAddressType SourceAddress,
                                  COSEMAddressType DestinationAddress,
                                  InvokeIdAndPriorityType InvokeID,
                                  COSEMPriority Priority,
                                  COSEMServiceClass ServiceClass,
                                  const Cosem_Attribute_Descriptor& AttributeDescriptor) : 
            APPBaseCallbackParameter(SourceAddress, DestinationAddress),
            m_Type(get_request_normal),
            m_InvokeIDAndPriority(InvokeID | Priority | ServiceClass)
        {
            m_Parameter = AttributeDescriptor;
        }
        APPGetRequestOrIndication(COSEMAddressType SourceAddress,
                                  COSEMAddressType DestinationAddress,
                                  InvokeIdAndPriorityType InvokeIDAndPriority,
                                  const Cosem_Attribute_Descriptor& AttributeDescriptor) : 
            APPBaseCallbackParameter(SourceAddress, DestinationAddress),
            m_Type(get_request_normal), 
            m_InvokeIDAndPriority(InvokeIDAndPriority)
        {
            m_Parameter = AttributeDescriptor;
        }
        APPGetRequestOrIndication(COSEMAddressType SourceAddress,
                                  COSEMAddressType DestinationAddress,
                                  InvokeIdAndPriorityType InvokeID,
                                  uint32_t BlockNumber) : 
            APPBaseCallbackParameter(SourceAddress, DestinationAddress),
            m_Type(get_request_next), 
            m_InvokeIDAndPriority(InvokeID)
        {
            m_Parameter = BlockNumber;
        }

        enum GetRequestType : int8_t
        {
            get_request_normal    = 1,
            get_request_next      = 2,
            get_request_with_list = 3
        }                              m_Type;        
        InvokeIdAndPriorityType        m_InvokeIDAndPriority;
        RequestParameter               m_Parameter;
    };

    struct APPGetConfirmOrResponse : public APPBaseCallbackParameter
    {
        static const uint16_t ID = 0x2004;
        APPGetConfirmOrResponse(COSEMAddressType SourceAddress,
                                COSEMAddressType DestinationAddress,
                                InvokeIdAndPriorityType InvokeID,
                                const DLMSVector& Data) : 
            APPBaseCallbackParameter(SourceAddress, DestinationAddress),
            m_Type(get_response_normal),
            m_InvokeIDAndPriority(InvokeID), 
            m_Data(Data)
        {
        }
        enum GetResponseType : int8_t
        {
            get_response_normal    = 1,
            get_response_next      = 2,
            get_response_with_list = 3
        }                               m_Type;        
        InvokeIdAndPriorityType         m_InvokeIDAndPriority;
        DLMSVector                      m_Data;
    };
    
    using GetRequestEventData = COSEMEventData<APPGetRequestOrIndication>;
    using GetResponseEventData = COSEMEventData<APPGetConfirmOrResponse>;
   
    class COSEMClient : public COSEM
    {
    public:
        COSEMClient() = delete;
        COSEMClient(COSEMAddressType ClientAddress);
        virtual ~COSEMClient();
        //
        // COSEM
        //
        virtual COSEMRunResult Process();
        //
        // COSEM-OPEN Service
        //
        bool OpenRequest(const APPOpenRequestOrIndication& Parameters);
        void RegisterOpenConfirm(CallbackFunction Callback);
        //
        // COSEM-GET Service
        //
        bool GetRequest(const APPGetRequestOrIndication& Parameters);
        void RegisterGetConfirm(CallbackFunction Callback);

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
        // APDU Handlers
        //
        virtual bool AARQ_Handler(const IAPDUPtr& pAPDU);
        virtual bool AARE_Handler(const IAPDUPtr& pAPDU);
        virtual bool GET_Request_Handler(const IAPDUPtr& pAPDU);
        virtual bool GET_Response_Handler(const IAPDUPtr& pAPDU);
        
    };
    
    class COSEMServer : public COSEM, public LogicalDevice
    {
    public:
        COSEMServer() = delete;
        COSEMServer(COSEMAddressType SAP);
        virtual ~COSEMServer();
        //
        // COSEM
        //
        virtual COSEMRunResult Process();
        //
        // COSEM-OPEN Service
        //
        void RegisterOpenIndication(CallbackFunction Callback);
        bool OpenResponse(const APPOpenConfirmOrResponse& Parameters);
        //
        // COSEM-GET Service
        //
        void RegisterGetIndication(LogicalDevice * pLogicalDevice);
        bool GetResponse(const APPGetConfirmOrResponse& Parameters);

    protected:
        //
        // Server Overrides
        //
        //
        // State Machine
        //
        void ST_Inactive_Handler(EventData * pData);
        void ST_Idle_Handler(EventData * pData);
        void ST_Association_Pending_Handler(EventData * pData);
        void ST_Association_Release_Pending_Handler(EventData * pData);
        void ST_Associated_Handler(EventData * pData);
        //
        // APDU Handlers
        //
        virtual bool AARQ_Handler(const IAPDUPtr& pAPDU);
        virtual bool AARE_Handler(const IAPDUPtr& pAPDU);
        virtual bool GET_Request_Handler(const IAPDUPtr& pAPDU);
        virtual bool GET_Response_Handler(const IAPDUPtr& pAPDU);
       
    };
    
}
