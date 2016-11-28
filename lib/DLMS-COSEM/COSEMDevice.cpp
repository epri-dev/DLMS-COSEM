#include <algorithm>

#include "COSEMDevice.h"
#include "COSEM.h"

namespace EPRI
{
    COSEMDevice::COSEMDevice()
    {
    }
    
    COSEMDevice::~COSEMDevice()
    {
    }
        
    void COSEMDevice::RegisterLogicalDevice(COSEMServer * pServer)
    {
        m_LogicalDevices[pServer->SAP()] = pServer;
    }

    void COSEMDevice::RegisterTransport(Transport * pXPort)
    {
        std::for_each(m_LogicalDevices.begin(),
            m_LogicalDevices.end(), 
            [&pXPort](LogicalDeviceMap::value_type& Value)
            {
                Value.second->RegisterTransport(pXPort);
            });
    }
    
}