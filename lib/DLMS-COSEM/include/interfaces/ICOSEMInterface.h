#pragma once

#include <limits>
#include <memory>
#include <map>

#include "COSEMData.h"
#include "COSEMAttribute.h"
#include "COSEMMethod.h"
#include "COSEMObjectInstanceID.h"
#include "COSEMTypes.h"

namespace EPRI
{
    
    class ICOSEMObject;
    
    class ICOSEMInterface
    {
        using COSEMAttributeMap = std::map<ObjectAttributeIdType, ICOSEMAttribute *>;
        friend class ICOSEMObject;
        
    public:
        ICOSEMInterface() = delete;
        ICOSEMInterface(uint16_t class_id, 
            uint8_t version, 
            uint16_t CardinalityMin = 0, 
            uint16_t CardinalityMax = std::numeric_limits<uint16_t>::max());
        virtual ~ICOSEMInterface();
            
        enum Attributes : ObjectAttributeIdType
        {
            ATTRIBUTE_0  = 0,
            LOGICAL_NAME = 1
        };
        
        COSEMAttribute<LOGICAL_NAME, OctetStringSchema, 0x00> logical_name;
            
    protected:
        virtual void RegisterAttribute(ICOSEMAttribute * pAttribute);
        virtual ICOSEMAttribute * FindAttribute(ObjectAttributeIdType AttributeId) const;
        inline bool HasAttribute(ObjectAttributeIdType AttributeId) const
        {
            return FindAttribute(AttributeId) != nullptr;
        }

        const uint16_t    m_class_id;
        const uint8_t     m_version;
        const uint16_t    m_CardinalityMin;
        const uint16_t    m_CardinalityMax;
        static uint16_t   m_CardinalityCounter;
        COSEMAttributeMap m_Attributes;
            
    };
    
    class ICOSEMObject;
    class SelectiveAccess;
    
    class ICOSEMObject
    {
    public:
        ICOSEMObject() = delete;
        ICOSEMObject(const COSEMObjectInstanceCriteria& Criteria, 
            uint16_t ShortNameBase = std::numeric_limits<uint16_t>::max());
        virtual ~ICOSEMObject();
        virtual bool Supports(const Cosem_Attribute_Descriptor& Descriptor) const;
        virtual bool Get(DLMSVector * pData,
            const Cosem_Attribute_Descriptor& Descriptor, 
            SelectiveAccess * pSelectiveAccess = nullptr);
            
    protected:
        virtual ICOSEMAttribute * FindAttribute(ObjectAttributeIdType AttributeId) const;
        virtual bool InternalGet(ICOSEMAttribute * pAttribute, 
            const Cosem_Attribute_Descriptor& Descriptor, 
            SelectiveAccess * pSelectiveAccess) = 0;

        const COSEMObjectInstanceCriteria m_InstanceCriteria;
        const uint16_t                    m_ShortNameBase;
        
    };
}