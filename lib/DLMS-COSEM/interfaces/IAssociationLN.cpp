//	0	0	40	0	0	255	Current association
// 	0	0	42	0	0	255	COSEM Logical device name (16 Byte OCTET-STRING; 3 = MANU; 13 = UNIQUE)

#include "interfaces/IAssociationLN.h"

namespace EPRI
{
    COSEM_BEGIN_SCHEMA(IAssociationLN_0::Object_List_Schema)
        
    COSEM_END_SCHEMA
    COSEM_BEGIN_SCHEMA(IAssociationLN_0::Assoc_Partners_Schema)
        COSEM_BEGIN_STRUCTURE
            COSEM_INTEGER_TYPE
            COSEM_LONG_UNSIGNED_TYPE
        COSEM_END_STRUCTURE
    COSEM_END_SCHEMA
    COSEM_BEGIN_SCHEMA(IAssociationLN_0::App_Context_Name_Schema)
        COSEM_BEGIN_CHOICE
            COSEM_BEGIN_STRUCTURE
                COSEM_UNSIGNED_TYPE
                COSEM_UNSIGNED_TYPE
                COSEM_LONG_UNSIGNED_TYPE
                COSEM_UNSIGNED_TYPE
                COSEM_UNSIGNED_TYPE
                COSEM_UNSIGNED_TYPE
                COSEM_UNSIGNED_TYPE
            COSEM_END_STRUCTURE
            COSEM_OCTET_STRING_TYPE
        COSEM_END_CHOICE
    COSEM_END_SCHEMA
    COSEM_BEGIN_SCHEMA(IAssociationLN_0::xDLMS_Schema)
        COSEM_BEGIN_STRUCTURE
            COSEM_BIT_STRING_TYPE
            COSEM_LONG_UNSIGNED_TYPE
            COSEM_LONG_UNSIGNED_TYPE
            COSEM_UNSIGNED_TYPE
            COSEM_INTEGER_TYPE
            COSEM_OCTET_STRING_TYPE
        COSEM_END_STRUCTURE
    COSEM_END_SCHEMA
    COSEM_BEGIN_SCHEMA(IAssociationLN_0::Mechanism_Schema)
        COSEM_BEGIN_CHOICE
            COSEM_BEGIN_STRUCTURE
                COSEM_UNSIGNED_TYPE
                COSEM_UNSIGNED_TYPE
                COSEM_LONG_UNSIGNED_TYPE
                COSEM_UNSIGNED_TYPE
                COSEM_UNSIGNED_TYPE
                COSEM_UNSIGNED_TYPE
                COSEM_UNSIGNED_TYPE
            COSEM_END_STRUCTURE
            COSEM_OCTET_STRING_TYPE
        COSEM_END_CHOICE
    COSEM_END_SCHEMA
    COSEM_BEGIN_SCHEMA(IAssociationLN_0::Assoc_Status_Schema)
        COSEM_ENUM_TYPE
        (
            { 
                IAssociationLN_0::AssociationState::non_associated,
                IAssociationLN_0::AssociationState::association_pending,
                IAssociationLN_0::AssociationState::associated
            }
        )
    COSEM_END_SCHEMA
 
    IAssociationLN_0::IAssociationLN_0()
        : ICOSEMInterface(CLSID_IAssociationLN, 0, 0, 1)
    {
        COSEM_BEGIN_ATTRIBUTES
            COSEM_ATTRIBUTE(object_list)
            COSEM_ATTRIBUTE(associated_partners_id)
            COSEM_ATTRIBUTE(application_context_name)
            COSEM_ATTRIBUTE(xDLMS_context_type)
            COSEM_ATTRIBUTE(mechanism_name)
            COSEM_ATTRIBUTE(secret)
            COSEM_ATTRIBUTE(association_status)
            COSEM_ATTRIBUTE(security_setup_reference)
        COSEM_END_ATTRIBUTES
    }
    
    IAssociationLN_0::~IAssociationLN_0()
    {
    }
    //
    // IAssociationLNObject
    //
    IAssociationLNObject::IAssociationLNObject(const COSEMObjectInstanceCriteria& OIDCriteria, 
        uint16_t ShortNameBase /* = std::numeric_limits<uint16_t>::max() */)
        : ICOSEMObject(OIDCriteria, ShortNameBase)
    {
    }
        
    IAssociationLNObject::~IAssociationLNObject()
    {
    }
    
}