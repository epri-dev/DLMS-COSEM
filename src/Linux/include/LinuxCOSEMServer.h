#pragma once

#include "COSEM.h"
#include "COSEMDevice.h"
#include "interfaces/IData.h"
#include "interfaces/IClock.h"

namespace EPRI
{
    class LinuxData : public IDataObject
    {
    public:
        LinuxData();
  
    protected:
        virtual bool InternalGet(ICOSEMAttribute * pAttribute, 
            const Cosem_Attribute_Descriptor& Descriptor, 
            SelectiveAccess * pSelectiveAccess) final;
        
    };

    class LinuxClock : public IClockObject
    {
    public:
        LinuxClock();
        
    protected:
        virtual bool InternalGet(ICOSEMAttribute * pAttribute, 
            const Cosem_Attribute_Descriptor& Descriptor, 
            SelectiveAccess * pSelectiveAccess) final;        
    };
    
    class LinuxManagementDevice : public COSEMServer
    {
    public:
        LinuxManagementDevice();
        virtual ~LinuxManagementDevice();
        
    protected:
        LinuxClock  m_Clock;
        LinuxData   m_Data;

    };
    
    class LinuxCOSEMServer : public COSEMDevice
    {
    public:
        LinuxCOSEMServer();
        virtual ~LinuxCOSEMServer();
        
    protected:
        LinuxManagementDevice m_Management;
        
    };
}
