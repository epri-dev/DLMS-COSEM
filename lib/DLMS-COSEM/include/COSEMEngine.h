#pragma once

#include <list>

#include "COSEMAddress.h"
#include "COSEMSecurity.h"
#include "COSEMObjectInstanceID.h"
#include "hdlc/modcnt.h"

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
        
        virtual bool Process() = 0;
        
    protected:
        Transport * m_pTransport;
        
    };
    
    class COSEMClientEngine : public COSEMEngine
    {
    public:
        typedef InvokeIdAndPriorityType GetToken; 
        typedef InvokeIdAndPriorityType SetToken; 
            
        typedef struct _Options : public COSEMEngine::Options
        {
            COSEMAddressType m_Address;            
            _Options(COSEMAddressType Address)
                : m_Address(Address)
            {
            }
            
        } Options;

        COSEMClientEngine() = delete;
        COSEMClientEngine(const Options& Opt, Transport * pXPort);
        virtual ~COSEMClientEngine();
        //
        // COSEMEngine
        //
        virtual bool Process();
        
        virtual bool Open(COSEMAddressType ServerAddress, const COSEMSecurityOptions& Security);
        virtual bool OnOpenConfirmation(COSEMAddressType ServerAddress);
        virtual bool IsOpen() const;
        virtual bool Get(const Cosem_Attribute_Descriptor& Descriptor,
                         GetToken * pToken);    
        virtual bool OnGetConfirmation(GetToken Token,
                                       const DLMSVector& Data);
        virtual bool Set(const Cosem_Attribute_Descriptor& Descriptor,
                         const DLMSVector& Value,
                         SetToken * pToken);    
        virtual bool OnSetConfirmation(SetToken Token);
        virtual bool Release();
        virtual bool OnReleaseConfirmation(COSEMAddressType ServerAddress);
        virtual bool OnAbortIndication(COSEMAddressType ServerAddress);
        
    protected:
        enum ServiceID : uint8_t
        {
            SERVICE_GET    = 0,
            SERVICE_SET    = 1,
            SERVICE_ACTION = 2,
            SERVICE_COUNT  = 3
        };
        
        bool Client_OpenConfirmation(const BaseCallbackParameter& Parameters);
        bool Client_GetConfirmation(const BaseCallbackParameter& Parameters);
        bool Client_SetConfirmation(const BaseCallbackParameter& Parameters);
        bool Client_ReleaseConfirmation(const BaseCallbackParameter& Parameters);
        bool Client_AbortIndication(const BaseCallbackParameter& Parameters);
        
        virtual InvokeIdAndPriorityType CurrentInvokeID(ServiceID Service) const;
        virtual InvokeIdAndPriorityType GetAndIncrementInvokeID(ServiceID Service);
        
        COSEMClient                         m_Client;
        Options                             m_Options;
        ModCounter<ALLOWED_INVOCATION_IDS>  m_InvokeID[SERVICE_COUNT];
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
