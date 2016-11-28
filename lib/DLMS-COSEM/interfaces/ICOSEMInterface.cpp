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

    void ICOSEMInterface::RegisterAttribute(ICOSEMAttribute * pAttribute)
    {
        m_Attributes[pAttribute->AttributeID] = pAttribute;
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

    ICOSEMAttribute * ICOSEMObject::FindAttribute(ObjectAttributeIdType AttributeId) const
    {
        const ICOSEMInterface * pInterface = dynamic_cast<const ICOSEMInterface *>(this);
        if (pInterface)
        {
            return pInterface->FindAttribute(AttributeId);
        }
        return nullptr;        
    }

    bool ICOSEMObject::Get(DLMSVector * pData,
        const Cosem_Attribute_Descriptor& Descriptor, 
        SelectiveAccess * pSelectiveAccess /*= nullptr*/)
    {
        bool              RetVal = false;
        ICOSEMAttribute * pAttribute = FindAttribute(Descriptor.attribute_id);
        if (pAttribute)
        {
            pAttribute->Clear();
            switch (Descriptor.attribute_id)
            {
            case ICOSEMInterface::ATTRIBUTE_0:
                //
                // UNSUPPORTED
                //
                return false;
            case ICOSEMInterface::LOGICAL_NAME:
                RetVal = pAttribute->Append(Descriptor.instance_id);
                break;
            default:
                RetVal = InternalGet(pAttribute, Descriptor, pSelectiveAccess);
            }
            if (RetVal)
            {
                pAttribute->GetBytes(pData);
            }
        }
        return RetVal;
    }
    
}