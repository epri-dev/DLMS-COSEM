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
        virtual void OnOpenConfirmation(COSEMAddressType ServerAddress);
        virtual bool IsOpen() const;
        virtual bool Get(const Cosem_Attribute_Descriptor& Descriptor,
                         GetToken * pToken);    
        virtual void OnGetConfirmation(GetToken Token,
                                       const DLMSVector& Data);
        virtual bool Close();
        
        
    protected:
        bool Client_OpenConfirmation(const BaseCallbackParameter& Parameters);
        bool Client_GetConfirmation(const BaseCallbackParameter& Parameters);
        
        COSEMClient      m_Client;
        Options          m_Options;
        modcounter       m_InvokeID;
        COSEMAddressType m_ServerAddress = INVALID_ADDRESS;
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
