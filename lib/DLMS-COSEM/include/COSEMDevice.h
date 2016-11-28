#pragma once

#include <map>

#include "COSEMAddress.h"

namespace EPRI
{
    class Transport;
    
#define SERVER_BEGIN_LOGICAL_DEVICES
        
#define SERVER_LOGICAL_DEVICE(LD)\
        RegisterLogicalDevice(&LD);
        
#define SERVER_END_LOGICAL_DEVICES
    
    class COSEMServer;

    class COSEMDevice
    {
    public:
        COSEMDevice();
        virtual ~COSEMDevice();
        
        virtual void RegisterLogicalDevice(COSEMServer * pServer);
        virtual void RegisterTransport(Transport * pXPort);
        
    protected:
        typedef std::map<COSEMAddressType, COSEMServer *> LogicalDeviceMap;
        LogicalDeviceMap                                  m_LogicalDevices;
    };

}
