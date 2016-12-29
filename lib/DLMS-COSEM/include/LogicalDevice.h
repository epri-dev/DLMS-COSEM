#pragma once

#include <cstdint>
#include <vector>
#include <list>

#include "COSEM.h"
#include "interfaces/ICOSEMInterface.h"
#include "interfaces/IAssociationLN.h"
#include "COSEMSecurity.h"

namespace EPRI
{
#define LOGICAL_DEVICE_BEGIN_OBJECTS
        
#define LOGICAL_DEVICE_OBJECT(OBJ)\
        RegisterObject(&OBJ);
        
#define LOGICAL_DEVICE_END_OBJECTS
    
    class COSEMServer;
    class APPBaseCallbackParameter;
    class APPOpenRequestOrIndication;
    class APPOpenConfirmOrResponse;
    class APPGetRequestOrIndication;
    class APPSetRequestOrIndication;
    class APPActionRequestOrIndication;
    class APPReleaseRequestOrIndication;
    class APPReleaseConfirmOrResponse;
    
    struct AssociationContext
    {
        using AssociationStatusType = IAssociationLN::AssociationState;
        
        AssociationContext(const APPOpenConfirmOrResponse& Response);
        //
        // Partners
        //
        COSEMAddressType        m_ClientSAP;
        COSEMAddressType        m_ServerSAP;
        //
        // Application Context
        //
        COSEMSecurityOptions    m_SecurityOptions;
        //
        // xDLMS Context Information
        //
        xDLMS::Context          m_xDLMS;
        //
        // Status Information
        //
        AssociationStatusType   m_Status;
        
    };
    
    class Association : public IAssociationLNObject
    {
    public:
        Association();
        virtual ~Association();

        virtual size_t AvailableAssociations() const;
        virtual bool RegisterAssociation(const APPOpenConfirmOrResponse& Response);
        virtual bool ReleaseAssociation(const APPReleaseConfirmOrResponse& Response);
        virtual const AssociationContext * GetAssociationContext(
            const APPBaseCallbackParameter& Parameter);
        COSEMAddressType GetAssociatedAddress() const;
        virtual void ReleaseTransientAssociations();
        
    protected:
        using AssociationInfoList = std::list<AssociationContext>;
        virtual APDUConstants::Data_Access_Result InternalGet(const AssociationContext& Context,
            ICOSEMAttribute * pAttribute, 
            const Cosem_Attribute_Descriptor& Descriptor, 
            SelectiveAccess * pSelectiveAccess) final;
        AssociationContext * GetAssociationContextByIndex(int Index);
        AssociationContext * GetAssociationContextByAddress(COSEMAddressType Address);
        
        AssociationInfoList m_Associations;
    };
    
    class LogicalDevice
    {
    public:
        LogicalDevice() = delete;
        LogicalDevice(COSEMServer * pServer);
        virtual ~LogicalDevice();
        
        virtual bool InitiateOpen(const APPOpenRequestOrIndication& Request, bool UpperLayerAllowed);
        virtual bool InitiateGet(const APPGetRequestOrIndication& Request, bool UpperLayerAllowed);
        virtual bool InitiateSet(const APPSetRequestOrIndication& Request, bool UpperLayerAllowed);
        virtual bool InitiateAction(const APPActionRequestOrIndication& Request, bool UpperLayerAllowed);
        virtual bool InitiateRelease(const APPReleaseRequestOrIndication& Request, bool UpperLayerAllowed);
        virtual COSEMAddressType SAP() const;
        virtual COSEMAddressType GetAssociatedAddress() const;
        
    protected:
        virtual void RegisterObject(ICOSEMObject * pObject);
        virtual ssize_t FindObject(const Cosem_Attribute_Descriptor& Descriptor) const;
        virtual ssize_t FindObject(const Cosem_Method_Descriptor& Descriptor) const;
        virtual bool IsForMe(const APPBaseCallbackParameter& Parameter) const;
        virtual void ReleaseTransientAssociations();

        std::vector<ICOSEMObject *>  m_Objects;
        Association                  m_Association;
        COSEMServer *                m_pServer = nullptr;
        
    };
}
