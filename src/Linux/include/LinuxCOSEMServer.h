#pragma once

#include "COSEM.h"
#include "COSEMDevice.h"
#include "COSEMEngine.h"
#include "interfaces/IData.h"
#include "interfaces/IClock.h"

namespace EPRI
{
    class LinuxData : public IDataObject
    {
    public:
        LinuxData();
  
    protected:
        virtual APDUConstants::Data_Access_Result InternalGet(ICOSEMAttribute * pAttribute, 
            const Cosem_Attribute_Descriptor& Descriptor, 
            SelectiveAccess * pSelectiveAccess) final;
        virtual APDUConstants::Data_Access_Result InternalSet(ICOSEMAttribute * pAttribute, 
            const Cosem_Attribute_Descriptor& Descriptor, 
            const DLMSVector& Data,
            SelectiveAccess * pSelectiveAccess) final;
        
        std::string m_Values[10];
        
    };

    class LinuxClock : public IClockObject
    {
    public:
        LinuxClock();
        
    protected:
        virtual APDUConstants::Data_Access_Result InternalGet(ICOSEMAttribute * pAttribute, 
            const Cosem_Attribute_Descriptor& Descriptor, 
            SelectiveAccess * pSelectiveAccess) final; 
        virtual APDUConstants::Action_Result InternalAction(ICOSEMMethod * pMethod, 
            const Cosem_Method_Descriptor& Descriptor, 
            const DLMSOptional<DLMSVector>& Parameters,
            DLMSVector * pReturnValue = nullptr) final;
        
        
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
    
    class LinuxCOSEMDevice : public COSEMDevice
    {
    public:
        LinuxCOSEMDevice();
        virtual ~LinuxCOSEMDevice();
        
    protected:
        LinuxManagementDevice m_Management;
        
    };
    
    class LinuxCOSEMServerEngine : public COSEMServerEngine
    {
    public:
        LinuxCOSEMServerEngine() = delete;
        LinuxCOSEMServerEngine(const Options& Opt, Transport * pXPort);
        virtual ~LinuxCOSEMServerEngine();
        
    protected:
        LinuxCOSEMDevice    m_Device;
    };
}
