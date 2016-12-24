#include "STM32COSEMServer.h"
#include "COSEMAddress.h"

namespace EPRI
{
    //
    // Data
    //
    STM32Data::STM32Data()
        : IDataObject({ 0, 0, 96, 1, { 0, 9 }, 255 })
    {
        for (int Index = 0; Index < 10; ++Index)
        {
            m_Values[Index] = "STM32DATA" + std::to_string(Index);
        }
    }

    APDUConstants::Data_Access_Result STM32Data::InternalGet(const AssociationContext& Context,
        ICOSEMAttribute * pAttribute, 
        const Cosem_Attribute_Descriptor& Descriptor, 
        SelectiveAccess * pSelectiveAccess)
    {
        pAttribute->SelectChoice(COSEMDataType::VISIBLE_STRING);
        pAttribute->Append(m_Values[Descriptor.instance_id.GetValueGroup(EPRI::COSEMObjectInstanceID::VALUE_GROUP_E)]);
        return APDUConstants::Data_Access_Result::success;
    }
    
    APDUConstants::Data_Access_Result STM32Data::InternalSet(const AssociationContext& Context,
        ICOSEMAttribute * pAttribute, 
        const Cosem_Attribute_Descriptor& Descriptor, 
        const DLMSVector& Data,
        SelectiveAccess * pSelectiveAccess)
    {
        APDUConstants::Data_Access_Result RetVal = APDUConstants::Data_Access_Result::temporary_failure;
        try
        {
            DLMSValue Value;
            
            RetVal = ICOSEMObject::InternalSet(Context, pAttribute, Descriptor, Data, pSelectiveAccess);
            if (APDUConstants::Data_Access_Result::success == RetVal &&
                pAttribute->GetNextValue(&Value) == COSEMType::GetNextResult::VALUE_RETRIEVED)
            {
                m_Values[Descriptor.instance_id.GetValueGroup(EPRI::COSEMObjectInstanceID::VALUE_GROUP_E)] =
                    DLMSValueGet<std::string>(Value);
                RetVal = APDUConstants::Data_Access_Result::success;
            }
            else
            {
                RetVal = APDUConstants::Data_Access_Result::type_unmatched;
            }
        }
        catch (...) 
        {
            RetVal = APDUConstants::Data_Access_Result::type_unmatched;
        }
        return RetVal;
    }
    //
    // Clock
    //
    STM32Clock::STM32Clock()
        : IClockObject({ 0, 0, 1, 0, 0, 255 })
    {
    }

    APDUConstants::Data_Access_Result STM32Clock::InternalGet(const AssociationContext& Context,
        ICOSEMAttribute * pAttribute, 
        const Cosem_Attribute_Descriptor& Descriptor, 
        SelectiveAccess * pSelectiveAccess)
    {
        switch (pAttribute->AttributeID)
        {
        case ATTR_TIME:
        case ATTR_TIME_ZONE:
        case ATTR_STATUS:
        case ATTR_DST_BEGIN:
        case ATTR_DST_END:
        case ATTR_DST_DEVIATION:
        case ATTR_DST_ENABLED:
        case ATTR_CLOCK_BASE:
        default:
            break;
        }
        //
        // TODO
        //
        return APDUConstants::Data_Access_Result::object_unavailable;
    }

    APDUConstants::Action_Result STM32Clock::InternalAction(const AssociationContext& Context,
        ICOSEMMethod * pMethod, 
        const Cosem_Method_Descriptor& Descriptor, 
        const DLMSOptional<DLMSVector>& Parameters,
        DLMSVector * pReturnValue /*= nullptr*/)
    {
        switch (pMethod->MethodID)
        {
        case METHOD_ADJUST_TO_QUARTER:
        case METHOD_ADJUST_TO_MEAS_PERIOD:
        case METHOD_ADJUST_TO_MINUTE:
        case METHOD_ADJUST_TO_PRESET_TIME:
        case METHOD_PRESET_ADJUSTING_TIME:
        case METHOD_SHIFT_TIME:
        default:
            break;
        }
        //
        // TODO
        //
        return APDUConstants::Action_Result::object_unavailable;
    }
    //
    // Logical Device
    //
    STM32ManagementDevice::STM32ManagementDevice()
        : COSEMServer(ReservedAddresses::MANAGEMENT)
    {
        LOGICAL_DEVICE_BEGIN_OBJECTS
            LOGICAL_DEVICE_OBJECT(m_Clock)
            LOGICAL_DEVICE_OBJECT(m_Data)
        LOGICAL_DEVICE_END_OBJECTS
    }
    
    STM32ManagementDevice::~STM32ManagementDevice()
    {
    }
    //
    // COSEM Device
    //
    STM32COSEMDevice::STM32COSEMDevice()
    {
        SERVER_BEGIN_LOGICAL_DEVICES
            SERVER_LOGICAL_DEVICE(m_Management)
        SERVER_END_LOGICAL_DEVICES
    }

    STM32COSEMDevice::~STM32COSEMDevice()
    {
    }
    //
    // COSEM Engine
    //
    STM32COSEMServerEngine::STM32COSEMServerEngine(const Options& Opt, Transport * pXPort)
        : COSEMServerEngine(Opt, pXPort)
    {
        ENGINE_BEGIN_DEVICES
            ENGINE_DEVICE(m_Device)
        ENGINE_END_DEVICES    
    }
    
    STM32COSEMServerEngine::~STM32COSEMServerEngine()
    {
    }
    
}