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
    class Association;
    class AssociationContext;
    
    class ICOSEMInterface
    {
        using COSEMAttributeMap = std::map<ObjectAttributeIdType, ICOSEMAttribute *>;
        using COSEMMethodMap = std::map<ObjectAttributeIdType, ICOSEMMethod *>;
        friend class ICOSEMObject;
        
    public:
        ICOSEMInterface() = delete;
        ICOSEMInterface(uint16_t class_id, 
            uint8_t version, 
            uint16_t CardinalityMin = 0, 
            uint16_t CardinalityMax = std::numeric_limits<uint16_t>::max());
        virtual ~ICOSEMInterface();
        
        enum InterfaceProperties : uint8_t
        {
            CLASS_ID,
            VERSION,
            CARDINALITY_MIN,
            CARDINALITY_MAX
        };
        
        virtual uint16_t GetPropertyValue(InterfaceProperties Property) const;
            
        enum Attributes : ObjectAttributeIdType
        {
            ATTRIBUTE_0  = 0,
            LOGICAL_NAME = 1
        };
        
        COSEMAttribute<LOGICAL_NAME, OctetStringSchema, 0x00> logical_name;
            
    protected:
        virtual void RegisterAttribute(ICOSEMAttribute * pAttribute);
        virtual void RegisterMethod(ICOSEMMethod * pMethod);
        virtual ICOSEMAttribute * FindAttribute(ObjectAttributeIdType AttributeId) const;
        virtual ICOSEMMethod * FindMethod(ObjectAttributeIdType MethodId) const;
        inline bool HasAttribute(ObjectAttributeIdType AttributeId) const
        {
            return FindAttribute(AttributeId) != nullptr;
        }
        inline bool HasMethod(ObjectAttributeIdType MethodId) const
        {
            return FindMethod(MethodId) != nullptr;
        }

        const uint16_t    m_class_id;
        const uint8_t     m_version;
        const uint16_t    m_CardinalityMin;
        const uint16_t    m_CardinalityMax;
        static uint16_t   m_CardinalityCounter;
        COSEMAttributeMap m_Attributes;
        COSEMMethodMap    m_Methods;
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
        virtual bool Supports(const Cosem_Method_Descriptor& Descriptor) const;
        virtual APDUConstants::Data_Access_Result Get(const AssociationContext& Context,
            DLMSVector * pData,
            const Cosem_Attribute_Descriptor& Descriptor, 
            SelectiveAccess * pSelectiveAccess = nullptr);
        virtual APDUConstants::Data_Access_Result Set(const AssociationContext& Context,
            const Cosem_Attribute_Descriptor& Descriptor, 
            const DLMSVector& Data,
            SelectiveAccess * pSelectiveAccess = nullptr);
        virtual APDUConstants::Action_Result Action(const AssociationContext& Context,
            const Cosem_Method_Descriptor& Descriptor, 
            const DLMSOptional<DLMSVector>& Parameters,
            DLMSVector * pReturnValue = nullptr);
            
    protected:
        virtual ICOSEMAttribute * FindAttribute(ObjectAttributeIdType AttributeId) const;
        virtual ICOSEMMethod * FindMethod(ObjectAttributeIdType MethodId) const;
        
        virtual APDUConstants::Data_Access_Result InternalGet(const AssociationContext& Context,
            ICOSEMAttribute * pAttribute, 
            const Cosem_Attribute_Descriptor& Descriptor, 
            SelectiveAccess * pSelectiveAccess) = 0;
        virtual APDUConstants::Data_Access_Result InternalSet(const AssociationContext& Context,
            ICOSEMAttribute * pAttribute, 
            const Cosem_Attribute_Descriptor& Descriptor, 
            const DLMSVector& Data,
            SelectiveAccess * pSelectiveAccess);
        virtual APDUConstants::Action_Result InternalAction(const AssociationContext& Context,
            ICOSEMMethod * pMethod, 
            const Cosem_Method_Descriptor& Descriptor, 
            const DLMSOptional<DLMSVector>& Parameters,
            DLMSVector * pReturnValue = nullptr);

        const COSEMObjectInstanceCriteria m_InstanceCriteria;
        const uint16_t                    m_ShortNameBase;
        
    };
}