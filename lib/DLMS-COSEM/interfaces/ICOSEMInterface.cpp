#include "interfaces/ICOSEMInterface.h"

namespace EPRI
{
    //
    // ICOSEMInterface
    //
    ICOSEMInterface::ICOSEMInterface(uint16_t class_id,
            uint8_t version, 
            uint16_t CardinalityMin /* = 0 */,
            uint16_t CardinalityMax /* = std::numeric_limits<uint16_t>::max() */) : 
        m_class_id(class_id), 
        m_version(version), 
        m_CardinalityMin(CardinalityMin), 
        m_CardinalityMax(CardinalityMax)
    {
        COSEM_BEGIN_ATTRIBUTES
            COSEM_ATTRIBUTE(logical_name)
        COSEM_END_ATTRIBUTES
    }
    
    ICOSEMInterface::~ICOSEMInterface()
    {
    }

    uint16_t ICOSEMInterface::GetPropertyValue(InterfaceProperties Property) const
    {
        switch (Property)
        {
        case CLASS_ID:
            return m_class_id;
        case VERSION:
            return m_version;
        case CARDINALITY_MIN:
            return m_CardinalityMin;
        case CARDINALITY_MAX :
            return m_CardinalityMax;
        default:
            break;
        }
        return 0;
    }

    void ICOSEMInterface::RegisterAttribute(ICOSEMAttribute * pAttribute)
    {
        pAttribute->m_pInterface = this;
        m_Attributes[pAttribute->AttributeID] = pAttribute;
    }
    
    void ICOSEMInterface::RegisterMethod(ICOSEMMethod * pMethod)
    {
        pMethod->m_pInterface = this;
        m_Methods[pMethod->MethodID] = pMethod;
    }
            
    ICOSEMAttribute * ICOSEMInterface::FindAttribute(ObjectAttributeIdType AttributeId) const
    {
        COSEMAttributeMap::const_iterator it = m_Attributes.find(AttributeId);
        if (it != m_Attributes.cend())
        {
            return it->second;
        }
        return nullptr;
    }
    
    ICOSEMMethod * ICOSEMInterface::FindMethod(ObjectAttributeIdType MethodId) const
    {
        COSEMMethodMap::const_iterator it = m_Methods.find(MethodId);
        if (it != m_Methods.cend())
        {
            return it->second;
        }
        return nullptr;
    }
    //
    // ICOSEMObject
    //
    ICOSEMObject::ICOSEMObject(const COSEMObjectInstanceCriteria& Criteria, 
        uint16_t ShortNameBase /* = std::numeric_limits<uint16_t>::max()*/) : 
        m_InstanceCriteria(Criteria), 
        m_ShortNameBase(ShortNameBase) 
    {
    }
    
    ICOSEMObject::~ICOSEMObject()
    {
    }
    
    bool ICOSEMObject::Supports(const Cosem_Attribute_Descriptor& Descriptor) const
    {
        if (m_InstanceCriteria.Match(Descriptor.instance_id))
        {
            const ICOSEMInterface * pInterface = dynamic_cast<const ICOSEMInterface *>(this);
            if (pInterface && 
                Descriptor.class_id == pInterface->m_class_id &&
                pInterface->HasAttribute(Descriptor.attribute_id))
            {
                return true;
            }
        }
        return false;
    }

    bool ICOSEMObject::Supports(const Cosem_Method_Descriptor& Descriptor) const
    {
        if (m_InstanceCriteria.Match(Descriptor.instance_id))
        {
            const ICOSEMInterface * pInterface = dynamic_cast<const ICOSEMInterface *>(this);
            if (pInterface && 
                Descriptor.class_id == pInterface->m_class_id &&
                pInterface->HasMethod(Descriptor.method_id))
            {
                return true;
            }
        }
        return false;
    }

    ICOSEMAttribute * ICOSEMObject::FindAttribute(ObjectAttributeIdType AttributeId) const
    {
        const ICOSEMInterface * pInterface = dynamic_cast<const ICOSEMInterface *>(this);
        if (pInterface)
        {
            return pInterface->FindAttribute(AttributeId);
        }
        return nullptr;        
    }

    ICOSEMMethod * ICOSEMObject::FindMethod(ObjectAttributeIdType MethodId) const
    {
        const ICOSEMInterface * pInterface = dynamic_cast<const ICOSEMInterface *>(this);
        if (pInterface)
        {
            return pInterface->FindMethod(MethodId);
        }
        return nullptr;        
    }
    
    APDUConstants::Data_Access_Result ICOSEMObject::Get(DLMSVector * pData,
        const Cosem_Attribute_Descriptor& Descriptor, 
        SelectiveAccess * pSelectiveAccess /*= nullptr*/)
    {
        APDUConstants::Data_Access_Result RetVal = APDUConstants::Data_Access_Result::object_unavailable;
        ICOSEMAttribute *                 pAttribute = FindAttribute(Descriptor.attribute_id);
        if (pAttribute)
        {
            pAttribute->Clear();
            switch (Descriptor.attribute_id)
            {
            case ICOSEMInterface::ATTRIBUTE_0:
                //
                // UNSUPPORTED
                //
                RetVal = APDUConstants::Data_Access_Result::object_unavailable;
                break;
                
            case ICOSEMInterface::LOGICAL_NAME:
                if (pAttribute->Append(Descriptor.instance_id))
                {
                    RetVal = APDUConstants::Data_Access_Result::success;
                }
                else
                {
                    RetVal = APDUConstants::Data_Access_Result::temporary_failure;
                }
                break;
                
            default:
                RetVal = InternalGet(pAttribute, Descriptor, pSelectiveAccess);
                break;
            }
            if (APDUConstants::Data_Access_Result::success == RetVal)
            {
                pAttribute->GetBytes(pData);
            }
        }
        return RetVal;
    }
    
    APDUConstants::Data_Access_Result ICOSEMObject::Set(const Cosem_Attribute_Descriptor& Descriptor, 
        const DLMSVector& Data,
        SelectiveAccess * pSelectiveAccess /*= nullptr*/)
    {
        APDUConstants::Data_Access_Result RetVal = APDUConstants::Data_Access_Result::object_unavailable;
        ICOSEMAttribute *                 pAttribute = FindAttribute(Descriptor.attribute_id);
        if (pAttribute)
        {
            pAttribute->Clear();
            switch (Descriptor.attribute_id)
            {
            case ICOSEMInterface::ATTRIBUTE_0:
                //
                // UNSUPPORTED
                //
                RetVal = APDUConstants::Data_Access_Result::object_unavailable;
                break;
                
            case ICOSEMInterface::LOGICAL_NAME:
                //
                // READ-ONLY
                //
                RetVal = APDUConstants::Data_Access_Result::read_write_denied;
                break;
                
            default:
                RetVal = InternalSet(pAttribute, Descriptor, Data, pSelectiveAccess);
                break;
            }
        }
        return RetVal;
    }    

    APDUConstants::Action_Result ICOSEMObject::Action(const Cosem_Method_Descriptor& Descriptor, 
        const DLMSOptional<DLMSVector>& Parameters,
        DLMSVector * pReturnValue /* = nullptr*/)
    {
        APDUConstants::Action_Result RetVal = APDUConstants::Action_Result::object_unavailable;
        ICOSEMMethod *               pMethod = FindMethod(Descriptor.method_id);
        if (pMethod)
        {
            pMethod->Clear();
            RetVal = InternalAction(pMethod, Descriptor, Parameters, pReturnValue);
        }
        return RetVal;
    }   
    
    APDUConstants::Data_Access_Result ICOSEMObject::InternalSet(ICOSEMAttribute * pAttribute, 
        const Cosem_Attribute_Descriptor& /*Descriptor*/, 
        const DLMSVector& Data,
        SelectiveAccess * /*pSelectiveAccess*/)
    {
        if (pAttribute->Parse(Data))
        {
            return APDUConstants::Data_Access_Result::success;
        }
        else
        {
            return APDUConstants::Data_Access_Result::temporary_failure;
        }
    }

    APDUConstants::Action_Result ICOSEMObject::InternalAction(ICOSEMMethod * pMethod, 
        const Cosem_Method_Descriptor& Descriptor, 
        const DLMSOptional<DLMSVector>& Parameters,
        DLMSVector * pReturnValue /* = nullptr*/)
    {
        return APDUConstants::Action_Result::object_unavailable;
    }
    
}