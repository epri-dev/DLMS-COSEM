#pragma once

#include "ICOSEMInterface.h"

namespace EPRI
{
    const uint16_t CLSID_IAssociationLN = 15;
    
    class IAssociationLN_0 : public ICOSEMInterface
    {
        COSEM_DEFINE_SCHEMA(Object_List_Schema)
        COSEM_DEFINE_SCHEMA(Assoc_Partners_Schema)
        COSEM_DEFINE_SCHEMA(App_Context_Name_Schema)
        COSEM_DEFINE_SCHEMA(xDLMS_Schema)
        COSEM_DEFINE_SCHEMA(Mechanism_Schema)
        COSEM_DEFINE_SCHEMA(Assoc_Status_Schema)

    public :
        IAssociationLN_0();
        virtual ~IAssociationLN_0();
        
        enum Attributes : ObjectAttributeIdType
        {
            ATTR_OBJ_LIST            = 2,
            ATTR_PARTNERS_ID         = 3,
            ATTR_APP_CON_NAME        = 4,
            ATTR_XDLMS_CON_INFO      = 5,
            ATTR_AUTH_MECH_NAME      = 6,
            ATTR_SECRET              = 7,
            ATTR_STATUS              = 8,
            ATTR_SECURITY_SETUP_REF  = 9
        };
        
        enum AssociationState : uint8_t
        {
            non_associated      = 0,
            association_pending = 1,
            associated          = 2
        };
        
        COSEMAttribute<ATTR_OBJ_LIST, Object_List_Schema, 0x08>           object_list;
        COSEMAttribute<ATTR_PARTNERS_ID, Assoc_Partners_Schema, 0x10>     associated_partners_id;
        COSEMAttribute<ATTR_APP_CON_NAME, App_Context_Name_Schema, 0x18>  application_context_name;
        COSEMAttribute<ATTR_XDLMS_CON_INFO, xDLMS_Schema, 0x20>           xDLMS_context_type;
        COSEMAttribute<ATTR_AUTH_MECH_NAME, Mechanism_Schema, 0x28>       mechanism_name;
        COSEMAttribute<ATTR_SECRET, OctetStringSchema, 0x30>              secret;
        COSEMAttribute<ATTR_STATUS, Assoc_Status_Schema, 0x38>            association_status;
        COSEMAttribute<ATTR_SECURITY_SETUP_REF, OctetStringSchema, 0x40>  security_setup_reference;
        //
        // All Methods Are Optional; Not Implementing at this Time
        //
    };
    
    typedef IAssociationLN_0 IAssociationLN;
    
    class IAssociationLNObject : public IAssociationLN, public ICOSEMObject
    {
    public:
        IAssociationLNObject() = delete;
        IAssociationLNObject(const COSEMObjectInstanceCriteria& OIDCriteria, 
            uint16_t ShortNameBase = std::numeric_limits<uint16_t>::max());
        virtual ~IAssociationLNObject();
    };

}