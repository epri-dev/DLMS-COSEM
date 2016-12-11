#pragma once

#include <cstdint>
#include <vector>
#include <map>

#include "COSEM.h"
#include "interfaces/ICOSEMInterface.h"

namespace EPRI
{
#define LOGICAL_DEVICE_BEGIN_OBJECTS
        
#define LOGICAL_DEVICE_OBJECT(OBJ)\
        RegisterObject(&OBJ);
        
#define LOGICAL_DEVICE_END_OBJECTS
    
    class COSEMServer;
    class APPGetRequestOrIndication;
    class APPSetRequestOrIndication;
    class APPActionRequestOrIndication;
    class APPReleaseRequestOrIndication;
    
    class LogicalDevice
    {
    public:
        LogicalDevice() = delete;
        LogicalDevice(COSEMServer * pServer);
        virtual ~LogicalDevice();
        
        virtual bool InitiateGet(const APPGetRequestOrIndication& Request, bool UpperLayerAllowed);
        virtual bool InitiateSet(const APPSetRequestOrIndication& Request, bool UpperLayerAllowed);
        virtual bool InitiateAction(const APPActionRequestOrIndication& Request, bool UpperLayerAllowed);
        virtual bool InitiateRelease(const APPReleaseRequestOrIndication& Request, bool UpperLayerAllowed);
        virtual bool Run();
        virtual COSEMAddressType SAP() const;
        
    protected:
        virtual bool IsRunning(InvokeIdAndPriorityType InvokeID);
        virtual void RegisterObject(ICOSEMObject * pObject);
        virtual ssize_t FindObject(const Cosem_Attribute_Descriptor& Descriptor) const;
        virtual ssize_t FindObject(const Cosem_Method_Descriptor& Descriptor) const;

        std::vector<ICOSEMObject *>         m_Objects;
        COSEMServer *                       m_pServer = nullptr;
        
    };
}
