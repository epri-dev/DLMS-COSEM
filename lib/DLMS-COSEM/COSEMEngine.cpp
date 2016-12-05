#include "IBaseLibrary.h"
#include "IDebug.h"
#include "COSEMEngine.h"
#include "COSEMDevice.h"
#include "Transport.h"

namespace EPRI
{
    //
    // COSEMClientEngine
    //
    COSEMClientEngine::COSEMClientEngine(const Options& Opt, Transport * pXPort) :
        COSEMEngine(pXPort), m_Client(Opt.m_Address), m_Options(Opt),
        m_InvokeID(ALLOWED_INVOCATION_IDS, 0)
    {
        m_Client.RegisterTransport(pXPort);
        m_Client.RegisterOpenConfirm(
            std::bind(&COSEMClientEngine::Client_OpenConfirmation, this, std::placeholders::_1));
        m_Client.RegisterGetConfirm(
            std::bind(&COSEMClientEngine::Client_GetConfirmation, this, std::placeholders::_1));
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
    
    bool COSEMClientEngine::Open(COSEMAddressType DestinationAddress, const COSEMSecurityOptions& Security)
    {
        return m_Client.OpenRequest(APPOpenRequestOrIndication(m_Options.m_Address, DestinationAddress, 
                                                               Security.Level, Security.Password));
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

    bool COSEMClientEngine::Get(const Cosem_Attribute_Descriptor& Descriptor,
                                GetToken * pToken)
    {
        bool RetVal = m_Client.IsOpen();
        if (RetVal)
        {
            RetVal = m_Client.GetRequest(
                           APPGetRequestOrIndication(m_Options.m_Address,
                                                     m_Client.GetAssociatedAddress(),
                                                     m_InvokeID, 
                                                     COSEMPriority::COSEM_PRIORITY_NORMAL,
                                                     COSEMServiceClass::COSEM_SERVICE_CONFIRMED,
                                                     Descriptor));
           
        }
        if (RetVal)
        {
            *pToken = m_InvokeID;
            ++m_InvokeID;
        }
        return RetVal;
    }
    
    bool COSEMClientEngine::OnGetConfirmation(GetToken Token,
                                              const DLMSVector& Data)
    {
        //
        // Default Handler Does Nothing
        //
        return true;
    }
    
    bool COSEMClientEngine::Release()
    {
        if (m_Client.IsOpen())
        {
            m_Client.ReleaseRequest(
                APPReleaseRequestOrIndication(m_Options.m_Address,
                m_Client.GetAssociatedAddress()));
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
    
    bool COSEMClientEngine::OnAbortIndication(COSEMAddressType /*ServerAddress*/)
    {
        //
        // Default Handler Does Nothing
        //
        return true;
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
        return OnGetConfirmation(COSEM_GET_INVOKE_ID(Confirmation.m_InvokeIDAndPriority),
                                 Confirmation.m_Data);
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