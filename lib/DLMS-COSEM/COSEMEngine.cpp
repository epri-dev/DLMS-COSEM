#include "IBaseLibrary.h"
#include "IDebug.h"
#include "COSEMEngine.h"
#include "COSEMDevice.h"
#include "Transport.h"

namespace EPRI
{

#define MAKE_TOKEN(SERVICE, INVOKEID)\
    ((SERVICE << 8) | INVOKEID)
    //
    // COSEMEngine
    //
    bool COSEMEngine::IsTransportConnected() const
    {
        return m_pTransport->IsConnected();
    }
    //
    // COSEMClientEngine
    //
    COSEMClientEngine::COSEMClientEngine(const Options& Opt, Transport * pXPort) :
        COSEMEngine(pXPort), m_Client(Opt.m_Address), m_Options(Opt)
    {
        m_Client.RegisterTransport(pXPort);
        m_Client.RegisterOpenConfirm(
            std::bind(&COSEMClientEngine::Client_OpenConfirmation, this, std::placeholders::_1));
        m_Client.RegisterGetConfirm(
            std::bind(&COSEMClientEngine::Client_GetConfirmation, this, std::placeholders::_1));
        m_Client.RegisterSetConfirm(
            std::bind(&COSEMClientEngine::Client_SetConfirmation, this, std::placeholders::_1));
        m_Client.RegisterActionConfirm(
            std::bind(&COSEMClientEngine::Client_ActionConfirmation, this, std::placeholders::_1));
        m_Client.RegisterReleaseConfirm(
            std::bind(&COSEMClientEngine::Client_ReleaseConfirmation, this, std::placeholders::_1));
        m_Client.RegisterAbortIndication(
            std::bind(&COSEMClientEngine::Client_AbortIndication, this, std::placeholders::_1));
    }
    
    COSEMClientEngine::~COSEMClientEngine()
    {
    }
    
    bool COSEMClientEngine::Process()
    {
        return true;
    }
    //
    // COSEM.Open Service
    //
    bool COSEMClientEngine::Open(COSEMAddressType DestinationAddress, const COSEMSecurityOptions& Security,
                                 const xDLMS::InitiateRequest& xDLMS)
    {
        return m_Client.OpenRequest(APPOpenRequestOrIndication(m_Options.m_Address, DestinationAddress, 
                                                               xDLMS, Security));
    }
    
    bool COSEMClientEngine::OnOpenConfirmation(COSEMAddressType /*ServerAddress*/)
    {
        //
        // Default Handler Does Nothing
        //
        return true;
    }

    bool COSEMClientEngine::IsOpen() const
    {
        return m_Client.IsOpen();
    }
    //
    // COSEM.Get Service
    //
    bool COSEMClientEngine::Get(const Cosem_Attribute_Descriptor& Descriptor,
                                RequestToken * pToken)
    {
        bool                    RetVal = m_Client.IsOpen();
        if (RetVal)
        {
            RetVal = m_Client.GetRequest(
                           APPGetRequestOrIndication(m_Options.m_Address,
                                                     m_Client.GetAssociatedAddress(),
                                                     CurrentInvokeID(SERVICE_GET), 
                                                     COSEMPriority::COSEM_PRIORITY_NORMAL,
                                                     COSEMServiceClass::COSEM_SERVICE_CONFIRMED,
                                                     Descriptor));
           
        }
        if (RetVal)
        {
            *pToken = MAKE_TOKEN(SERVICE_GET,
                                 GetAndIncrementInvokeID(SERVICE_GET));
            m_Responses.emplace(std::make_pair(*pToken, GetResponse(Descriptor)));
        }
        return RetVal;
    }
    
    bool COSEMClientEngine::OnGetConfirmation(RequestToken Token, const GetResponse& Response)
    {
        //
        // Default Handler Does Nothing
        //
        return true;
    }
    //
    // COSEM.Set Service
    //
    bool COSEMClientEngine::Set(const Cosem_Attribute_Descriptor& Descriptor,
                                const DLMSVector& Value,
                                RequestToken * pToken)    
    {
        bool RetVal = m_Client.IsOpen();
        if (RetVal)
        {
            RetVal = m_Client.SetRequest(
                           APPSetRequestOrIndication(m_Options.m_Address,
                                m_Client.GetAssociatedAddress(),
                                CurrentInvokeID(SERVICE_SET), 
                                COSEMPriority::COSEM_PRIORITY_NORMAL,
                                COSEMServiceClass::COSEM_SERVICE_CONFIRMED,
                                Descriptor,
                                Value));
           
        }
        if (RetVal)
        {
            *pToken = MAKE_TOKEN(SERVICE_SET,
                                 GetAndIncrementInvokeID(SERVICE_SET));
            m_Responses.emplace(std::make_pair(*pToken, SetResponse(Descriptor)));
        }
        return RetVal;
    }
    
    bool COSEMClientEngine::OnSetConfirmation(RequestToken Token, const SetResponse& Response)
    {
        //
        // Default Handler Does Nothing
        //
        return true;
    }    
    //
    // COSEM.Action Service
    //
    bool COSEMClientEngine::Action(const Cosem_Method_Descriptor& Descriptor,
        const DLMSOptional<DLMSVector>& Parameters,
        RequestToken * pToken)    
    {
        bool RetVal = m_Client.IsOpen();
        if (RetVal)
        {
            RetVal = m_Client.ActionRequest(
                           APPActionRequestOrIndication(m_Options.m_Address,
                                m_Client.GetAssociatedAddress(),
                                CurrentInvokeID(SERVICE_SET), 
                                COSEMPriority::COSEM_PRIORITY_NORMAL,
                                COSEMServiceClass::COSEM_SERVICE_CONFIRMED,
                                Descriptor,
                                Parameters));
           
        }
        if (RetVal)
        {
            *pToken = MAKE_TOKEN(SERVICE_ACTION,
                                 GetAndIncrementInvokeID(SERVICE_ACTION));
            m_Responses.emplace(std::make_pair(*pToken, ActionResponse(Descriptor)));
        }
        return RetVal;
    }
    
    bool COSEMClientEngine::OnActionConfirmation(RequestToken Token, const ActionResponse& Response)
    {
        //
        // Default Handler Does Nothing
        //
        return true;
    } 
    //
    // COSEM.Release Service
    //
    bool COSEMClientEngine::Release(const xDLMS::InitiateRequest& xDLMS, bool UseRLRE /* = true */)
    {
        if (m_Client.IsOpen())
        {
            m_Client.ReleaseRequest(
                APPReleaseRequestOrIndication(m_Options.m_Address,
                    m_Client.GetAssociatedAddress(),
                    xDLMS,
                    UseRLRE));
        }
        return true;    
    }
    
    bool COSEMClientEngine::OnReleaseConfirmation(COSEMAddressType /*ServerAddress*/)
    {
        //
        // Default Handler Does Nothing
        //
        return true;
    }
    //
    // COSEM.Abort Service
    //
    bool COSEMClientEngine::OnAbortIndication(COSEMAddressType /*ServerAddress*/)
    {
        //
        // Default Handler Does Nothing
        //
        return true;
    }
    //
    // Protected Methods
    //
    InvokeIdAndPriorityType COSEMClientEngine::CurrentInvokeID(ServiceID Service) const
    {
        return m_InvokeID[Service];
    }
    
    InvokeIdAndPriorityType COSEMClientEngine::GetAndIncrementInvokeID(ServiceID Service)
    {
        return m_InvokeID[Service]++;
    }
    
    bool COSEMClientEngine::Client_OpenConfirmation(const BaseCallbackParameter& Parameters)
    {
        const APPOpenConfirmOrResponse& Confirmation = 
            dynamic_cast<const APPOpenConfirmOrResponse&>(Parameters);
        return OnOpenConfirmation(Confirmation.m_SourceAddress);
    }
    
    bool COSEMClientEngine::Client_GetConfirmation(const BaseCallbackParameter& Parameters)
    {
        const APPGetConfirmOrResponse& Confirmation = dynamic_cast<const APPGetConfirmOrResponse&>(Parameters);
        RequestToken                   Token = MAKE_TOKEN(SERVICE_GET, 
                                                          COSEM_GET_INVOKE_ID(Confirmation.m_InvokeIDAndPriority));
        try
        {
            GetResponse& Response = PickupResponse<GetResponse>(Token);
            Response.Result = Confirmation.m_Result;
            Response.ResultValid = true;
            
            return OnGetConfirmation(Token, Response);
        }
        catch (...)
        {
        }
        return false;        
    }

    bool COSEMClientEngine::Client_SetConfirmation(const BaseCallbackParameter& Parameters)
    {
        const APPSetConfirmOrResponse& Confirmation = dynamic_cast<const APPSetConfirmOrResponse&>(Parameters);
        RequestToken                   Token = MAKE_TOKEN(SERVICE_SET, 
                                                          COSEM_GET_INVOKE_ID(Confirmation.m_InvokeIDAndPriority));
        try
        {
            SetResponse& Response = PickupResponse<SetResponse>(Token);
            Response.Result = Confirmation.m_Result;
            Response.ResultValid = true;
            
            return OnSetConfirmation(Token, Response);
        }
        catch (...)
        {
        }
        return false;        
    }

    bool COSEMClientEngine::Client_ActionConfirmation(const BaseCallbackParameter& Parameters)
    {
        const APPActionConfirmOrResponse& Confirmation = dynamic_cast<const APPActionConfirmOrResponse&>(Parameters);
        RequestToken                   Token = MAKE_TOKEN(SERVICE_ACTION, 
                                                          COSEM_GET_INVOKE_ID(Confirmation.m_InvokeIDAndPriority));
        try
        {
            ActionResponse& Response = PickupResponse<ActionResponse>(Token);
            Response.Result = Confirmation.m_Result;
            Response.ResultValid = true;
            
            return OnActionConfirmation(Token, Response);
        }
        catch (...)
        {
        }
        return false;        
        
    }
    
    bool COSEMClientEngine::Client_ReleaseConfirmation(const BaseCallbackParameter& Parameters)
    {
        const APPReleaseConfirmOrResponse& Confirmation = dynamic_cast<const APPReleaseConfirmOrResponse&>(Parameters);
        return OnReleaseConfirmation(Confirmation.m_SourceAddress);
    }
    
    bool COSEMClientEngine::Client_AbortIndication(const BaseCallbackParameter& Parameters)
    {
        const APPAbortIndication& Indication = dynamic_cast<const APPAbortIndication&>(Parameters);
        return OnAbortIndication(Indication.m_SourceAddress);
    }
    //
    // COSEMServerEngine
    //
    COSEMServerEngine::COSEMServerEngine(const Options& Opt, Transport * pXPort) :
        COSEMEngine(pXPort), m_Options(Opt)
    {
    }
    
    COSEMServerEngine::~COSEMServerEngine()
    {
    }

    bool COSEMServerEngine::Process()
    {
        return m_pTransport->Process();
    }
    
    void COSEMServerEngine::RegisterDevice(COSEMDevice * pDevice)
    {
        pDevice->RegisterTransport(m_pTransport);
        m_Devices.push_back(pDevice);
    }
    
    void COSEMServerEngine::UnregisterDevice(COSEMDevice * pDevice)
    {
        m_Devices.remove(pDevice);
    }
    
}