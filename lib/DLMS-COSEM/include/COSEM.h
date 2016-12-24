#pragma once

#include <functional>
#include <map>
#include <memory>

#include "interfaces/COSEMAttribute.h"
#include "Callback.h"
#include "Transport.h"
#include "StateMachine.h"
#include "APDU/APDUDefs.h"
#include "LogicalDevice.h"
#include "COSEMSecurity.h"

namespace EPRI
{
    class IAPDU;
    
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
    //
    // BaseCallbackParameter for COSEM
    //
    struct APPBaseCallbackParameter : public BaseCallbackParameter
    {
        APPBaseCallbackParameter(COSEMAddressType SourceAddress,
            COSEMAddressType DestinationAddress)
            : m_SourceAddress(SourceAddress)
            , m_DestinationAddress(DestinationAddress)
        {
        }
        COSEMAddressType  m_SourceAddress;
        COSEMAddressType  m_DestinationAddress;
    };
    //
    // ABORT Service
    //
    struct APPAbortIndication : public APPBaseCallbackParameter
    {
        static const uint16_t ID = 0x2000;
        
        APPAbortIndication(COSEMAddressType SourceAddress,
            COSEMAddressType DestinationAddress,
            uint32_t Diagnostic = 0)
            : APPBaseCallbackParameter(SourceAddress, DestinationAddress)
            , m_Diagnostic(Diagnostic)
        {
        }
        //
        // TODO - Make useful
        //
        uint32_t      m_Diagnostic;
        
    };

    using AbortIndicationEventData = COSEMEventData<APPAbortIndication>;    
    
    class COSEM : public Callback<bool, uint16_t>, public StateMachine
    {
    public:
        typedef int TRANSPORT_HANDLE;
        
        COSEM() = delete;
        COSEM(COSEMAddressType Address);
        virtual ~COSEM();
    	
        virtual COSEMRunResult Process() = 0;
        virtual TRANSPORT_HANDLE RegisterTransport(Transport * pTransport);
        virtual bool IsOpen() const;
        virtual COSEMAddressType GetAddress() const;
        virtual COSEMAddressType GetAssociatedAddress() const;
        //
        // COSEM-ABORT Service
        //
        void RegisterAbortIndication(CallbackFunction Callback);

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
            ST_MAX_STATES,
            ST_IGNORED = StateMachine::EVENT_IGNORED
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
        virtual bool SET_Request_Handler(const IAPDUPtr& pAPDU) = 0;
        virtual bool SET_Response_Handler(const IAPDUPtr& pAPDU) = 0;
        virtual bool ACTION_Request_Handler(const IAPDUPtr& pAPDU) = 0;
        virtual bool ACTION_Response_Handler(const IAPDUPtr& pAPDU) = 0;
        virtual bool RLRQ_Handler(const IAPDUPtr& pAPDU) = 0;
        virtual bool RLRE_Handler(const IAPDUPtr& pAPDU) = 0;
        //
        // Helpers
        //
        virtual Transport * GetTransport() const;
        
        std::map<TRANSPORT_HANDLE, Transport *> m_Transports;
        COSEMAddressType                        m_Address;
        COSEMAddressType                        m_AssociatedAddress = INVALID_ADDRESS;
        
    };
    //
    // OPEN Service
    //
    struct APPOpenConfirmOrResponse : public APPBaseCallbackParameter
    {
        static const uint16_t ID = 0x2001;
        using AssociationResultType = AARE::AssociationResult;
        using DiagnosticSourceType = AARE::AssociateDiagnosticChoice;
        using BaseDiagnosticType = int8_t;
        using UserDiagnosticType = AARE::AssociateDiagnosticUser;
        using ProviderDiagnosticType = AARE::AssociateDiagnosticProvider;
        using ApplicationContextType = DLMSOptional<ASNObjectIdentifier>;
        
        APPOpenConfirmOrResponse(COSEMAddressType SourceAddress,
                                 COSEMAddressType DestinationAddress,
                                 const xDLMS::InitiateResponse& xDLMS,
                                 const COSEMSecurityOptions& Security,
                                 AssociationResultType Result,
                                 DiagnosticSourceType DiagnosticSource,
                                 BaseDiagnosticType Diagnostic)
            : 
            APPBaseCallbackParameter(SourceAddress, DestinationAddress),
            m_Result(Result), m_SecurityOptions(Security),
            m_DiagnosticSource(DiagnosticSource), m_Diagnostic(Diagnostic),
            m_xDLMS(xDLMS)
        {
        }
        APPOpenConfirmOrResponse(AARE * pAARE);
        bool ToAPDU(AARE * pAARE);

        AssociationResultType   m_Result;
        DiagnosticSourceType    m_DiagnosticSource;
        BaseDiagnosticType      m_Diagnostic;
        xDLMS::InitiateResponse m_xDLMS;
        //
        // Mechanism and Security
        //
        COSEMSecurityOptions m_SecurityOptions;
        
    };
    
    struct APPOpenRequestOrIndication : public APPBaseCallbackParameter
    {
        static const uint16_t ID = 0x2002;
        APPOpenRequestOrIndication(COSEMAddressType SourceAddress,
                                   COSEMAddressType DestinationAddress,
                                   const xDLMS::InitiateRequest& xDLMS,
                                   const COSEMSecurityOptions& Security) : 
            APPBaseCallbackParameter(SourceAddress, DestinationAddress),
            m_xDLMS(xDLMS),
            m_SecurityOptions(Security)
        {
        }
        APPOpenRequestOrIndication(AARQ * pAARQ);
        bool ToAPDU(AARQ * pAARQ);
        //
        // Application Context Name Building
        //
        xDLMS::InitiateRequest  m_xDLMS;
        //
        // Mechanism and Security
        //
        COSEMSecurityOptions    m_SecurityOptions;
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
        using GetRequestType = Get_Request::Get_Request_Choice;

        APPGetRequestOrIndication(COSEMAddressType SourceAddress,
                                  COSEMAddressType DestinationAddress,
                                  InvokeIdAndPriorityType InvokeID,
                                  COSEMPriority Priority,
                                  COSEMServiceClass ServiceClass,
                                  const Cosem_Attribute_Descriptor& AttributeDescriptor) : 
            APPBaseCallbackParameter(SourceAddress, DestinationAddress),
            m_Type(GetRequestType::get_request_normal),
            m_InvokeIDAndPriority(InvokeID | Priority | ServiceClass)
        {
            m_Parameter = AttributeDescriptor;
        }
        APPGetRequestOrIndication(COSEMAddressType SourceAddress,
                                  COSEMAddressType DestinationAddress,
                                  InvokeIdAndPriorityType InvokeIDAndPriority,
                                  const Cosem_Attribute_Descriptor& AttributeDescriptor) : 
            APPBaseCallbackParameter(SourceAddress, DestinationAddress),
            m_Type(GetRequestType::get_request_normal), 
            m_InvokeIDAndPriority(InvokeIDAndPriority)
        {
            m_Parameter = AttributeDescriptor;
        }
        APPGetRequestOrIndication(COSEMAddressType SourceAddress,
                                  COSEMAddressType DestinationAddress,
                                  InvokeIdAndPriorityType InvokeID,
                                  uint32_t BlockNumber) : 
            APPBaseCallbackParameter(SourceAddress, DestinationAddress),
            m_Type(GetRequestType::get_request_next), 
            m_InvokeIDAndPriority(InvokeID)
        {
            m_Parameter = BlockNumber;
        }
        
        GetRequestType                 m_Type;        
        InvokeIdAndPriorityType        m_InvokeIDAndPriority;
        RequestParameter               m_Parameter;
    };

    struct APPGetConfirmOrResponse : public APPBaseCallbackParameter
    {
        static const uint16_t ID = 0x2004;
        using GetResponseType = Get_Response::Get_Response_Choice;
        
        APPGetConfirmOrResponse(COSEMAddressType SourceAddress,
                                COSEMAddressType DestinationAddress,
                                InvokeIdAndPriorityType InvokeID,
                                const Get_Data_Result& Result) : 
            APPBaseCallbackParameter(SourceAddress, DestinationAddress),
            m_Type(GetResponseType::get_response_normal),
            m_InvokeIDAndPriority(InvokeID), 
            m_Result(Result)
        {
        }
        GetResponseType         m_Type;        
        InvokeIdAndPriorityType m_InvokeIDAndPriority;
        Get_Data_Result         m_Result;
    };
    
    using GetRequestEventData = COSEMEventData<APPGetRequestOrIndication>;
    using GetResponseEventData = COSEMEventData<APPGetConfirmOrResponse>;
    //
    // SET Service
    //
    struct APPSetRequestOrIndication : public APPBaseCallbackParameter
    {
        static const uint16_t ID = 0x2005;
        typedef variant<Cosem_Attribute_Descriptor, uint32_t> RequestParameter;
        using SetRequestType = Set_Request::Set_Request_Choice;

        APPSetRequestOrIndication(COSEMAddressType SourceAddress,
                COSEMAddressType DestinationAddress,
                InvokeIdAndPriorityType InvokeID,
                COSEMPriority Priority,
                COSEMServiceClass ServiceClass,
                const Cosem_Attribute_Descriptor& AttributeDescriptor,
                const DLMSVector& Value) : 
            APPBaseCallbackParameter(SourceAddress, DestinationAddress), 
            m_Type(SetRequestType::set_request_normal), 
            m_InvokeIDAndPriority(InvokeID | Priority | ServiceClass),
            m_Value(Value)
        {
            m_Parameter = AttributeDescriptor;
        }
        APPSetRequestOrIndication(COSEMAddressType SourceAddress,
                COSEMAddressType DestinationAddress,
                InvokeIdAndPriorityType InvokeIDAndPriority,
                const Cosem_Attribute_Descriptor& AttributeDescriptor,
                const DLMSVector& Value) : 
            APPBaseCallbackParameter(SourceAddress, DestinationAddress), 
            m_Type(SetRequestType::set_request_normal), 
            m_InvokeIDAndPriority(InvokeIDAndPriority),
            m_Value(Value)
        {
            m_Parameter = AttributeDescriptor;
        }
        
        SetRequestType                 m_Type;        
        InvokeIdAndPriorityType        m_InvokeIDAndPriority;
        RequestParameter               m_Parameter;
        DLMSVector                     m_Value;
    };

    struct APPSetConfirmOrResponse : public APPBaseCallbackParameter
    {
        static const uint16_t ID = 0x2006;
        using SetResponseType = Set_Response::Set_Response_Choice;
        
        APPSetConfirmOrResponse(COSEMAddressType SourceAddress,
                COSEMAddressType DestinationAddress,
                InvokeIdAndPriorityType InvokeID,
                APDUConstants::Data_Access_Result Result) : 
            APPBaseCallbackParameter(SourceAddress, DestinationAddress), 
            m_Type(SetResponseType::set_response_normal), 
            m_InvokeIDAndPriority(InvokeID), m_Result(Result)
        {
        }
        SetResponseType                   m_Type;        
        InvokeIdAndPriorityType           m_InvokeIDAndPriority;
        APDUConstants::Data_Access_Result m_Result;
    };
    
    using SetRequestEventData = COSEMEventData<APPSetRequestOrIndication>;
    using SetResponseEventData = COSEMEventData<APPSetConfirmOrResponse>;    
    //
    // ACTION Service
    //
    struct APPActionRequestOrIndication : public APPBaseCallbackParameter
    {
        static const uint16_t ID = 0x2007;
        typedef variant<Cosem_Method_Descriptor, uint32_t> RequestParameter;
        using ActionRequestType = Action_Request::Action_Request_Choice;

        APPActionRequestOrIndication(COSEMAddressType SourceAddress,
            COSEMAddressType DestinationAddress,
            InvokeIdAndPriorityType InvokeID,
            COSEMPriority Priority,
            COSEMServiceClass ServiceClass,
            const Cosem_Method_Descriptor& MethodDescriptor,
            const DLMSOptional<DLMSVector>& ActionParams) : 
            APPBaseCallbackParameter(SourceAddress, DestinationAddress), 
            m_Type(ActionRequestType::action_request_normal), 
            m_InvokeIDAndPriority(InvokeID | Priority | ServiceClass), 
            m_ActionParameters(ActionParams)
        {
            m_Parameter = MethodDescriptor;
        }
        APPActionRequestOrIndication(COSEMAddressType SourceAddress,
            COSEMAddressType DestinationAddress,
            InvokeIdAndPriorityType InvokeIDAndPriority,
            const Cosem_Method_Descriptor& MethodDescriptor,
            const DLMSOptional<DLMSVector>& ActionParams) : 
            APPBaseCallbackParameter(SourceAddress, DestinationAddress), 
            m_Type(ActionRequestType::action_request_normal), 
            m_InvokeIDAndPriority(InvokeIDAndPriority), 
            m_ActionParameters(ActionParams)
        {
            m_Parameter = MethodDescriptor;
        }
        
        ActionRequestType              m_Type;        
        InvokeIdAndPriorityType        m_InvokeIDAndPriority;
        RequestParameter               m_Parameter;
        DLMSOptional<DLMSVector>       m_ActionParameters;
    };

    struct APPActionConfirmOrResponse : public APPBaseCallbackParameter
    {
        static const uint16_t ID = 0x2008;
        using ActionResponseType = Action_Response::Action_Response_Choice;
        
        APPActionConfirmOrResponse(COSEMAddressType SourceAddress,
            COSEMAddressType DestinationAddress,
            InvokeIdAndPriorityType InvokeID,
            APDUConstants::Action_Result Result) : 
            APPBaseCallbackParameter(SourceAddress, DestinationAddress), 
            m_Type(ActionResponseType::action_response_normal), 
            m_InvokeIDAndPriority(InvokeID), 
            m_Result(Result)
        {
        }
        ActionResponseType                m_Type;        
        InvokeIdAndPriorityType           m_InvokeIDAndPriority;
        APDUConstants::Action_Result      m_Result;
    };
    
    using ActionRequestEventData = COSEMEventData<APPActionRequestOrIndication>;
    using ActionResponseEventData = COSEMEventData<APPActionConfirmOrResponse>;      
    //
    // RELEASE Service
    //
    struct APPReleaseConfirmOrResponse : public APPBaseCallbackParameter
    {
        static const uint16_t ID = 0x2009;
        using ReleaseReason = RLRE::ReleaseResponseReason;
        
        APPReleaseConfirmOrResponse(COSEMAddressType SourceAddress,
            COSEMAddressType DestinationAddress,
            bool UseRLRQRLRE = true,
            ReleaseReason Reason = ReleaseReason::normal)
            : APPBaseCallbackParameter(SourceAddress, DestinationAddress)
            , m_UseRLRQRLRE(UseRLRQRLRE)
            , m_Reason(Reason)
        {
        }

        bool          m_UseRLRQRLRE;
        ReleaseReason m_Reason;    

    };
    
    struct APPReleaseRequestOrIndication : public APPBaseCallbackParameter
    {
        static const uint16_t ID = 0x200A;
        using ReleaseReason = RLRQ::ReleaseRequestReason;
        
        APPReleaseRequestOrIndication(COSEMAddressType SourceAddress,
            COSEMAddressType DestinationAddress,
            bool UseRLRQRLRE = true,
            ReleaseReason Reason = ReleaseReason::normal) : 
            APPBaseCallbackParameter(SourceAddress, DestinationAddress), 
            m_UseRLRQRLRE(UseRLRQRLRE), 
            m_Reason(Reason)
        {
        }
        
        bool          m_UseRLRQRLRE;
        ReleaseReason m_Reason;    
        
    };

    using ReleaseRequestEventData = COSEMEventData<APPReleaseRequestOrIndication>;
    using ReleaseResponseEventData = COSEMEventData<APPReleaseConfirmOrResponse>;
   
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
        //
        // COSEM-SET Service
        //
        bool SetRequest(const APPSetRequestOrIndication& Parameters);
        void RegisterSetConfirm(CallbackFunction Callback);
        //
        // COSEM-ACTION Service
        //
        bool ActionRequest(const APPActionRequestOrIndication& Parameters);
        void RegisterActionConfirm(CallbackFunction Callback);
        //
        // COSEM-RELEASE Service
        //
        bool ReleaseRequest(const APPReleaseRequestOrIndication& Parameters);
        void RegisterReleaseConfirm(CallbackFunction Callback);

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
        virtual bool SET_Request_Handler(const IAPDUPtr& pAPDU);
        virtual bool SET_Response_Handler(const IAPDUPtr& pAPDU);
        virtual bool ACTION_Request_Handler(const IAPDUPtr& pAPDU);
        virtual bool ACTION_Response_Handler(const IAPDUPtr& pAPDU);
        virtual bool RLRQ_Handler(const IAPDUPtr& pAPDU);
        virtual bool RLRE_Handler(const IAPDUPtr& pAPDU);
       
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
        bool OpenResponse(const APPOpenConfirmOrResponse& Parameters);
        //
        // COSEM-GET Service
        //
        bool GetResponse(const APPGetConfirmOrResponse& Parameters);
        //
        // COSEM-SET Service
        //
        bool SetResponse(const APPSetConfirmOrResponse& Parameters);
        //
        // COSEM-ACTION Service
        //
        bool ActionResponse(const APPActionConfirmOrResponse& Parameters);
        //
        // COSEM-RELEASE Service
        //
        bool ReleaseResponse(const APPReleaseConfirmOrResponse& Parameters);

    protected:
        //
        // Server Event Handlers
        //
        //
        // COSEM-OPEN Service
        //
        virtual bool OnOpenIndication(const APPOpenRequestOrIndication& Parameters);
        //
        // COSEM-GET Service
        //
        virtual bool OnGetIndication(const APPGetRequestOrIndication& Parameters);
        //
        // COSEM-SET Service
        //
        virtual bool OnSetIndication(const APPSetRequestOrIndication& Parameters);
        //
        // COSEM-ACTION Service
        //
        virtual bool OnActionIndication(const APPActionRequestOrIndication& Parameters);
        //
        // COSEM-RELEASE Service
        //
        virtual bool OnReleaseIndication(const APPReleaseRequestOrIndication& Parameters);
        //
        // COSEM-ABORT Service
        //
        virtual bool OnAbortIndication(const APPAbortIndication& Parameters);
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
        virtual bool SET_Request_Handler(const IAPDUPtr& pAPDU);
        virtual bool SET_Response_Handler(const IAPDUPtr& pAPDU);
        virtual bool ACTION_Request_Handler(const IAPDUPtr& pAPDU);
        virtual bool ACTION_Response_Handler(const IAPDUPtr& pAPDU);
        virtual bool RLRQ_Handler(const IAPDUPtr& pAPDU);
        virtual bool RLRE_Handler(const IAPDUPtr& pAPDU);
       
    };
    
}
