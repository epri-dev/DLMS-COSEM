#pragma once

#include "COSEM.h"
#include "COSEMDevice.h"
#include "COSEMEngine.h"
#include "interfaces/IData.h"
#include "interfaces/IClock.h"

namespace EPRI
{
    class STM32Data : public IDataObject
    {
    public:
        STM32Data();
  
    protected:
        virtual APDUConstants::Data_Access_Result InternalGet(const AssociationContext& Context,
            ICOSEMAttribute * pAttribute, 
            const Cosem_Attribute_Descriptor& Descriptor, 
            SelectiveAccess * pSelectiveAccess) final;
        virtual APDUConstants::Data_Access_Result InternalSet(const AssociationContext& Context,
            ICOSEMAttribute * pAttribute, 
            const Cosem_Attribute_Descriptor& Descriptor, 
            const DLMSVector& Data,
            SelectiveAccess * pSelectiveAccess) final;
        
        std::string m_Values[10];
        
    };

    class STM32Clock : public IClockObject
    {
    public:
        STM32Clock();
        
    protected:
        virtual APDUConstants::Data_Access_Result InternalGet(const AssociationContext& Context,
            ICOSEMAttribute * pAttribute, 
            const Cosem_Attribute_Descriptor& Descriptor, 
            SelectiveAccess * pSelectiveAccess) final; 
        virtual APDUConstants::Action_Result InternalAction(const AssociationContext& Context,
            ICOSEMMethod * pMethod, 
            const Cosem_Method_Descriptor& Descriptor, 
            const DLMSOptional<DLMSVector>& Parameters,
            DLMSVector * pReturnValue = nullptr) final;
        
        
    };
    
    class STM32ManagementDevice : public COSEMServer
    {
    public:
        STM32ManagementDevice();
        virtual ~STM32ManagementDevice();
        
    protected:
        STM32Clock  m_Clock;
        STM32Data   m_Data;

    };
    
    class STM32COSEMDevice : public COSEMDevice
    {
    public:
        STM32COSEMDevice();
        virtual ~STM32COSEMDevice();
        
    protected:
        STM32ManagementDevice m_Management;
        
    };
    
    class STM32COSEMServerEngine : public COSEMServerEngine
    {
    public:
        STM32COSEMServerEngine() = delete;
        STM32COSEMServerEngine(const Options& Opt, Transport * pXPort);
        virtual ~STM32COSEMServerEngine();
        
    protected:
        STM32COSEMDevice    m_Device;
    };
}
