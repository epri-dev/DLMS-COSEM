#pragma once

#include <list>
#include <map>
#include <type_traits>

#include "COSEM.h"
#include "COSEMAddress.h"
#include "COSEMSecurity.h"
#include "COSEMObjectInstanceID.h"
#include "hdlc/modcnt.h"
#include "APDU/xDLMS.h"

namespace EPRI
{	 
    class Transport;
    class ICOSEMAttribute;
    class COSEMDevice;
    
    class COSEMEngine
    {
    public:
        typedef struct _Options
        {
            _Options()
            {
            }
            
        } Options;

        COSEMEngine() = delete;
        COSEMEngine(Transport * pXPort)
            : m_pTransport(pXPort)
        {
        }
        
        virtual ~COSEMEngine()
        {
        }
        
        virtual bool IsTransportConnected() const;
        virtual bool Process() = 0;
        
    protected:
        Transport * m_pTransport;
        
    };
    
    class COSEMClientEngine : public COSEMEngine
    {
    public:
        enum ServiceID : uint8_t
        {
            SERVICE_GET    = 0,
            SERVICE_SET    = 1,
            SERVICE_ACTION = 2,
            SERVICE_COUNT  = 3
        };
        
        typedef uint16_t RequestToken;
            
        typedef struct _Options : public COSEMEngine::Options
        {
            COSEMAddressType m_Address; 
            bool             m_LogicalNameReferencing;
            
            _Options(COSEMAddressType Address, bool LogicalNameReferencing = true)
                : m_Address(Address)
                , m_LogicalNameReferencing(LogicalNameReferencing)
            {
            }
            
        } Options;
        
        struct GetResponse
        {
            GetResponse(const Cosem_Attribute_Descriptor& D) :
                Descriptor(D)
            {
            }
            Cosem_Attribute_Descriptor        Descriptor;
            Get_Data_Result                   Result;
            bool                              ResultValid = false;
        };
        struct SetResponse
        {
            SetResponse(const Cosem_Attribute_Descriptor& D)
                : Descriptor(D)
            {
            }
            Cosem_Attribute_Descriptor        Descriptor;
            APDUConstants::Data_Access_Result Result;
            bool                              ResultValid = false;
        };
        struct ActionResponse
        {
            ActionResponse(const Cosem_Method_Descriptor& D)
                : Descriptor(D)
            {
            }
            Cosem_Method_Descriptor           Descriptor;
            APDUConstants::Action_Result      Result;
            bool                              ResultValid = false;
        };

        COSEMClientEngine() = delete;
        COSEMClientEngine(const Options& Opt, Transport * pXPort);
        virtual ~COSEMClientEngine();
        //
        // COSEMEngine
        //
        virtual bool Process();
        
        virtual bool Open(COSEMAddressType ServerAddress, const COSEMSecurityOptions& Security,
            const xDLMS::InitiateRequest& xDLMS);
        virtual bool OnOpenConfirmation(COSEMAddressType ServerAddress);
        virtual bool IsOpen() const;
        virtual bool Get(const Cosem_Attribute_Descriptor& Descriptor,
                         RequestToken * pToken);    
        virtual bool OnGetConfirmation(RequestToken Token, const GetResponse& Response);
        virtual bool Set(const Cosem_Attribute_Descriptor& Descriptor,
                         const DLMSVector& Value,
                         RequestToken * pToken);    
        virtual bool OnSetConfirmation(RequestToken Token, const SetResponse& Response);
        virtual bool Action(const Cosem_Method_Descriptor& Descriptor,
            const DLMSOptional<DLMSVector>& Parameters,
            RequestToken * pToken);    
        virtual bool OnActionConfirmation(RequestToken Token, const ActionResponse& Response);
        virtual bool Release();
        virtual bool OnReleaseConfirmation(COSEMAddressType ServerAddress);
        virtual bool OnAbortIndication(COSEMAddressType ServerAddress);
        
        template <typename TResponse>
            TResponse& PickupResponse(RequestToken Token)
            {
                ServiceID Service;
                if (std::is_same<GetResponse, TResponse>::value)
                    Service = SERVICE_GET;
                else if (std::is_same<SetResponse, TResponse>::value)
                    Service = SERVICE_SET;
                else if (std::is_same<ActionResponse, TResponse>::value)
                    Service = SERVICE_ACTION;
                ResponseMap::iterator it = m_Responses.find(Token);
                if (it != m_Responses.end() && it->second.which() == Service)
                {
                    return it->second.get<TResponse>();
                }
                throw std::out_of_range("Token not found");
            }
        
    protected:
        bool Client_OpenConfirmation(const BaseCallbackParameter& Parameters);
        bool Client_GetConfirmation(const BaseCallbackParameter& Parameters);
        bool Client_SetConfirmation(const BaseCallbackParameter& Parameters);
        bool Client_ActionConfirmation(const BaseCallbackParameter& Parameters);
        bool Client_ReleaseConfirmation(const BaseCallbackParameter& Parameters);
        bool Client_AbortIndication(const BaseCallbackParameter& Parameters);
        
        virtual InvokeIdAndPriorityType CurrentInvokeID(ServiceID Service) const;
        virtual InvokeIdAndPriorityType GetAndIncrementInvokeID(ServiceID Service);
        
        typedef variant<GetResponse, SetResponse, ActionResponse> ResponseTypes;
        typedef std::map<RequestToken, ResponseTypes>             ResponseMap;
        
        COSEMClient                            m_Client;
        Options                                m_Options;
        ModCounter<ALLOWED_INVOCATION_IDS>     m_InvokeID[SERVICE_COUNT];
        ResponseMap                            m_Responses;
    };
    
#define ENGINE_BEGIN_DEVICES
    
#define ENGINE_DEVICE(DEV)\
    RegisterDevice(&DEV);
    
#define ENGINE_END_DEVICES
    
    class COSEMServerEngine : public COSEMEngine
    {
    public:
        COSEMServerEngine() = delete;
        COSEMServerEngine(const Options& Opt, Transport * pXPort);
        virtual ~COSEMServerEngine();
        //
        // COSEMEngine
        //
        virtual bool Process();
        
    protected:
        virtual void RegisterDevice(COSEMDevice * pDevice);
        virtual void UnregisterDevice(COSEMDevice * pDevice);
       
        using COSEMDeviceList = std::list<COSEMDevice *>;
        COSEMDeviceList m_Devices;
        Options         m_Options;
    };
}
