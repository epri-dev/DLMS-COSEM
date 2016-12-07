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
                            DLMSVector Data;
                            //
                            // Get the data for this attribute and object
                            //
                            if (m_Objects[ObjectIndex]->Get(&Data, Request.m_Parameter.get<Cosem_Attribute_Descriptor>()))
                            {
                                //
                                // TODO - Add error cases...
                                //
                                return m_pServer->GetResponse(APPGetConfirmOrResponse(SAP(),
                                                                                      Request.m_SourceAddress,
                                                                                      Request.m_InvokeIDAndPriority,
                                                                                      Data));
                            }
                        }
                    }
                    break;
                    
                case APPGetRequestOrIndication::GetRequestType::get_request_next:
                    throw std::logic_error("get_request_next Not Implemented!");
                    
                case APPGetRequestOrIndication::GetRequestType::get_request_with_list:
                    throw std::logic_error("get_request_with_list Not Implemented!");
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
                            DLMSVector Data;
                            //
                            // Get the data for this attribute and object
                            //
                            if (m_Objects[ObjectIndex]->Set(Request.m_Parameter.get<Cosem_Attribute_Descriptor>(), Request.m_Value))
                            {
                                //
                                // TODO - Add error cases...
                                //
                                return m_pServer->SetResponse(APPSetConfirmOrResponse(SAP(),
                                    Request.m_SourceAddress,
                                    Request.m_InvokeIDAndPriority,
                                    APDUConstants::Data_Access_Result::success));
                            }
                        }
                    }
                    break;
                case APPSetRequestOrIndication::SetRequestType::set_request_with_first_datablock :
                    throw std::logic_error("set_request_with_first_datablock Not Implemented!");

                case APPSetRequestOrIndication::SetRequestType::set_request_with_datablock :
                    throw std::logic_error("set_request_with_datablock Not Implemented!");
                    
                case APPSetRequestOrIndication::SetRequestType::set_request_with_list :
                    throw std::logic_error("set_request_with_list Not Implemented!");

                case APPSetRequestOrIndication::SetRequestType::set_request_with_list_and_first_datablock :
                    throw std::logic_error("set_request_with_list_and_first_datablock Not Implemented!");
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

}