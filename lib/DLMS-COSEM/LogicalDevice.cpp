#include "COSEM.h"
#include "LogicalDevice.h"

namespace EPRI
{
    LogicalDevice::LogicalDevice(COSEMServer * pServer) :
        m_pServer(pServer)
    {
    }
    
    LogicalDevice::~LogicalDevice()
    {
    }

    bool LogicalDevice::InitiateGet(const APPGetRequestOrIndication& Request, bool UpperLayerAllowed)
    {
        if (SAP() == Request.m_DestinationAddress ||
            Request.m_DestinationAddress == ReservedAddresses::BROADCAST)
        {
            InvokeIdAndPriorityType InvokeID = COSEM_GET_INVOKE_ID(Request.m_InvokeIDAndPriority);

            // Check to see if we already have this invoke_id running
            if (IsRunning(InvokeID))
            {
                // TODO - Return some error
            }
            else 
            {
                switch (Request.m_Type)
                {
                case APPGetRequestOrIndication::GetRequestType::get_request_normal:
                    {
                        ssize_t ObjectIndex = 
                            FindObject(Request.m_Parameter.get<Cosem_Attribute_Descriptor>());
                        if (ObjectIndex != -1)
                        {
                            DLMSVector                        Data;
                            APDUConstants::Data_Access_Result Result = 
                                m_Objects[ObjectIndex]->Get(&Data, Request.m_Parameter.get<Cosem_Attribute_Descriptor>());
                            //
                            // Get the data for this attribute and object
                            //
                            return m_pServer->GetResponse(
                                APPGetConfirmOrResponse(SAP(),
                                                        Request.m_SourceAddress,
                                                        Request.m_InvokeIDAndPriority,
                                                        (Result == APDUConstants::Data_Access_Result::success ? 
                                                            Get_Data_Result(Data) : 
                                                            Result)));
                        }
                    }
                    break;
                    
                default:
                    throw std::logic_error("InitiateGet Type Not Implemented!");
                }
            }
        }
        return false;
    }
    
    bool LogicalDevice::InitiateSet(const APPSetRequestOrIndication& Request, bool UpperLayerAllowed)
    {
        if (SAP() == Request.m_DestinationAddress ||
            Request.m_DestinationAddress == ReservedAddresses::BROADCAST)
        {
            InvokeIdAndPriorityType InvokeID = COSEM_GET_INVOKE_ID(Request.m_InvokeIDAndPriority);

            // Check to see if we already have this invoke_id running
            if (IsRunning(InvokeID))
            {
                // TODO - Return some error
            }
            else 
            {
                switch (Request.m_Type)
                {
                case APPSetRequestOrIndication::SetRequestType::set_request_normal:
                    {
                        ssize_t ObjectIndex = 
                            FindObject(Request.m_Parameter.get<Cosem_Attribute_Descriptor>());
                        if (ObjectIndex != -1)
                        {
                            //
                            // Get the data for this attribute and object
                            //
                            return m_pServer->SetResponse(APPSetConfirmOrResponse(SAP(),
                                Request.m_SourceAddress,
                                Request.m_InvokeIDAndPriority,
                                m_Objects[ObjectIndex]->Set(Request.m_Parameter.get<Cosem_Attribute_Descriptor>(), Request.m_Value)));
                        }
                    }
                    break;

                default:
                    throw std::logic_error("InitiateSet Type Not Implemented!");

                }
            }
        }
        return false;
    }
    
    bool LogicalDevice::InitiateAction(const APPActionRequestOrIndication& Request, bool UpperLayerAllowed)
    {
        if (SAP() == Request.m_DestinationAddress ||
            Request.m_DestinationAddress == ReservedAddresses::BROADCAST)
        {
            InvokeIdAndPriorityType InvokeID = COSEM_GET_INVOKE_ID(Request.m_InvokeIDAndPriority);

            // Check to see if we already have this invoke_id running
            if (IsRunning(InvokeID))
            {
                // TODO - Return some error
            }
            else 
            {
                switch (Request.m_Type)
                {
                case APPActionRequestOrIndication::ActionRequestType::action_request_normal:
                    {
                        ssize_t ObjectIndex = 
                            FindObject(Request.m_Parameter.get<Cosem_Method_Descriptor>());
                        if (ObjectIndex != -1)
                        {
                            DLMSVector Data;
                            //
                            // Get the data for this attribute and object
                            //
                            return m_pServer->ActionResponse(APPActionConfirmOrResponse(SAP(),
                                Request.m_SourceAddress,
                                Request.m_InvokeIDAndPriority,
                                m_Objects[ObjectIndex]->Action(Request.m_Parameter.get<Cosem_Method_Descriptor>(), Request.m_ActionParameters)));
                        }
                    }
                    break;
                    
                default:
                    throw std::logic_error("InitiateAction Type Not Implemented!");

                }
            }
        }
        return false;
    }    
    
    bool LogicalDevice::InitiateRelease(const APPReleaseRequestOrIndication& Request, bool UpperLayerAllowed)
    {
        return m_pServer->ReleaseResponse(APPReleaseConfirmOrResponse(SAP(),
            Request.m_SourceAddress,
            Request.m_UseRLRQRLRE,
            UpperLayerAllowed ? APPReleaseConfirmOrResponse::ReleaseReason::normal : 
                  APPReleaseConfirmOrResponse::ReleaseReason::not_finished));
    }
    
    bool LogicalDevice::Run()
    {
    }

    COSEMAddressType LogicalDevice::SAP() const
    {
        return dynamic_cast<COSEM *>(m_pServer)->GetAddress();
    }

    bool LogicalDevice::IsRunning(InvokeIdAndPriorityType InvokeID)
    {
        //
        // TODO
        //
        return false;
    }

    void LogicalDevice::RegisterObject(ICOSEMObject * pObject)
    {
        m_Objects.push_back(pObject);
    }
    
    ssize_t LogicalDevice::FindObject(const Cosem_Attribute_Descriptor& Descriptor) const
    {
        ssize_t RetVal = 0;
        for (; RetVal < m_Objects.size(); ++RetVal)
        {
            if (m_Objects[RetVal]->Supports(Descriptor))
            {
                return RetVal;
            }
        }
        return -1;
    }
    
    ssize_t LogicalDevice::FindObject(const Cosem_Method_Descriptor& Descriptor) const
    {
        ssize_t RetVal = 0;
        for (; RetVal < m_Objects.size(); ++RetVal)
        {
            if (m_Objects[RetVal]->Supports(Descriptor))
            {
                return RetVal;
            }
        }
        return -1;
    }
    

}