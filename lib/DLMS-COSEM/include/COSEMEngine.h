// ===========================================================================
// Copyright (c) 2018, Electric Power Research Institute (EPRI)
// All rights reserved.
//
// DLMS-COSEM ("this software") is licensed under BSD 3-Clause license.
//
// Redistribution and use in source and binary forms, with or without modification,
// are permitted provided that the following conditions are met:
//
// *  Redistributions of source code must retain the above copyright notice, this
//    list of conditions and the following disclaimer.
//
// *  Redistributions in binary form must reproduce the above copyright notice,
//    this list of conditions and the following disclaimer in the documentation
//    and/or other materials provided with the distribution.
//
// *  Neither the name of EPRI nor the names of its contributors may
//    be used to endorse or promote products derived from this software without
//    specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
// ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
// WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
// IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
// INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
// NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
// PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
// WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY
// OF SUCH DAMAGE.
//
// This EPRI software incorporates work covered by the following copyright and permission
// notices. You may not use these works except in compliance with their respective
// licenses, which are provided below.
//
// These works are provided by the copyright holders and contributors "as is" and any express or
// implied warranties, including, but not limited to, the implied warranties of merchantability
// and fitness for a particular purpose are disclaimed.
//
// This software relies on the following libraries and licenses:
//
// ###########################################################################
// Boost Software License, Version 1.0
// ###########################################################################
//
// * asio v1.10.8 (https://sourceforge.net/projects/asio/files/)
//
// Boost Software License - Version 1.0 - August 17th, 2003
//
// Permission is hereby granted, free of charge, to any person or organization
// obtaining a copy of the software and accompanying documentation covered by
// this license (the "Software") to use, reproduce, display, distribute,
// execute, and transmit the Software, and to prepare derivative works of the
// Software, and to permit third-parties to whom the Software is furnished to
// do so, all subject to the following:
//
// The copyright notices in the Software and this entire statement, including
// the above license grant, this restriction and the following disclaimer,
// must be included in all copies of the Software, in whole or in part, and
// all derivative works of the Software, unless such copies or derivative
// works are solely in the form of machine-executable object code generated by
// a source language processor.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE, TITLE AND NON-INFRINGEMENT. IN NO EVENT
// SHALL THE COPYRIGHT HOLDERS OR ANYONE DISTRIBUTING THE SOFTWARE BE LIABLE
// FOR ANY DAMAGES OR OTHER LIABILITY, WHETHER IN CONTRACT, TORT OR OTHERWISE,
// ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
// DEALINGS IN THE SOFTWARE.
// 

#pragma once

#include <list>
#include <map>
#include <type_traits>

#include "COSEM.h"
#include "COSEMAddress.h"
#include "COSEMSecurity.h"
#include "COSEMObjectInstanceID.h"
#include "hdlc/modcnt.h"
#include "APDU/xDLMS.h"

namespace EPRI
{	 
    class Transport;
    class ICOSEMAttribute;
    class COSEMDevice;
    
    class COSEMEngine
    {
    public:
        typedef struct _Options
        {
            _Options()
            {
            }
            
        } Options;

        COSEMEngine() = delete;
        COSEMEngine(Transport * pXPort)
            : m_pTransport(pXPort)
        {
        }
        
        virtual ~COSEMEngine()
        {
        }
        
        virtual bool IsTransportConnected() const;
        virtual bool Process() = 0;
        
    protected:
        Transport * m_pTransport;
        
    };
    
    class COSEMClientEngine : public COSEMEngine
    {
    public:
        enum ServiceID : uint8_t
        {
            SERVICE_GET    = 0,
            SERVICE_SET    = 1,
            SERVICE_ACTION = 2,
            SERVICE_COUNT  = 3
        };
        
        typedef uint16_t RequestToken;
            
        typedef struct _Options : public COSEMEngine::Options
        {
            COSEMAddressType m_Address; 
            bool             m_LogicalNameReferencing;
            
            _Options(COSEMAddressType Address, bool LogicalNameReferencing = true)
                : m_Address(Address)
                , m_LogicalNameReferencing(LogicalNameReferencing)
            {
            }
            
        } Options;
        
        struct GetResponse
        {
            GetResponse(const Cosem_Attribute_Descriptor& D) :
                Descriptor(D)
            {
            }
            Cosem_Attribute_Descriptor        Descriptor;
            Get_Data_Result                   Result;
            bool                              ResultValid = false;
        };
        struct SetResponse
        {
            SetResponse(const Cosem_Attribute_Descriptor& D)
                : Descriptor(D)
            {
            }
            Cosem_Attribute_Descriptor        Descriptor;
            APDUConstants::Data_Access_Result Result;
            bool                              ResultValid = false;
        };
        struct ActionResponse
        {
            ActionResponse(const Cosem_Method_Descriptor& D)
                : Descriptor(D)
            {
            }
            Cosem_Method_Descriptor           Descriptor;
            APDUConstants::Action_Result      Result;
            bool                              ResultValid = false;
        };

        COSEMClientEngine() = delete;
        COSEMClientEngine(const Options& Opt, Transport * pXPort);
        virtual ~COSEMClientEngine();
        //
        // COSEMEngine
        //
        virtual bool Process();
        
        virtual bool Open(COSEMAddressType ServerAddress, const COSEMSecurityOptions& Security,
            const xDLMS::InitiateRequest& xDLMS);
        virtual bool OnOpenConfirmation(COSEMAddressType ServerAddress);
        virtual bool IsOpen() const;
        virtual bool Get(const Cosem_Attribute_Descriptor& Descriptor,
                         RequestToken * pToken);    
        virtual bool OnGetConfirmation(RequestToken Token, const GetResponse& Response);
        virtual bool Set(const Cosem_Attribute_Descriptor& Descriptor,
                         const DLMSVector& Value,
                         RequestToken * pToken);    
        virtual bool OnSetConfirmation(RequestToken Token, const SetResponse& Response);
        virtual bool Action(const Cosem_Method_Descriptor& Descriptor,
            const DLMSOptional<DLMSVector>& Parameters,
            RequestToken * pToken);    
        virtual bool OnActionConfirmation(RequestToken Token, const ActionResponse& Response);
        virtual bool Release(const xDLMS::InitiateRequest& xDLMS, bool UseRLRE = true);
        virtual bool OnReleaseConfirmation(COSEMAddressType ServerAddress);
        virtual bool OnAbortIndication(COSEMAddressType ServerAddress);
        
        template <typename TResponse>
            TResponse& PickupResponse(RequestToken Token)
            {
                ServiceID Service;
                if (std::is_same<GetResponse, TResponse>::value)
                    Service = SERVICE_GET;
                else if (std::is_same<SetResponse, TResponse>::value)
                    Service = SERVICE_SET;
                else if (std::is_same<ActionResponse, TResponse>::value)
                    Service = SERVICE_ACTION;
                ResponseMap::iterator it = m_Responses.find(Token);
                if (it != m_Responses.end() && it->second.index() == Service)
                {
                    return std::get<TResponse>(it->second);
                }
                throw std::out_of_range("Token not found");
            }
        
    protected:
        bool Client_OpenConfirmation(const BaseCallbackParameter& Parameters);
        bool Client_GetConfirmation(const BaseCallbackParameter& Parameters);
        bool Client_SetConfirmation(const BaseCallbackParameter& Parameters);
        bool Client_ActionConfirmation(const BaseCallbackParameter& Parameters);
        bool Client_ReleaseConfirmation(const BaseCallbackParameter& Parameters);
        bool Client_AbortIndication(const BaseCallbackParameter& Parameters);
        
        virtual InvokeIdAndPriorityType CurrentInvokeID(ServiceID Service) const;
        virtual InvokeIdAndPriorityType GetAndIncrementInvokeID(ServiceID Service);
        
        typedef std::variant<GetResponse, SetResponse, ActionResponse> ResponseTypes;
        typedef std::map<RequestToken, ResponseTypes>             ResponseMap;
        
        COSEMClient                            m_Client;
        Options                                m_Options;
        ModCounter<ALLOWED_INVOCATION_IDS>     m_InvokeID[SERVICE_COUNT];
        ResponseMap                            m_Responses;
    };
    
#define ENGINE_BEGIN_DEVICES
    
#define ENGINE_DEVICE(DEV)\
    RegisterDevice(&DEV);
    
#define ENGINE_END_DEVICES
    
    class COSEMServerEngine : public COSEMEngine
    {
    public:
        COSEMServerEngine() = delete;
        COSEMServerEngine(const Options& Opt, Transport * pXPort);
        virtual ~COSEMServerEngine();
        //
        // COSEMEngine
        //
        virtual bool Process();
        
    protected:
        virtual void RegisterDevice(COSEMDevice * pDevice);
        virtual void UnregisterDevice(COSEMDevice * pDevice);
       
        using COSEMDeviceList = std::list<COSEMDevice *>;
        COSEMDeviceList m_Devices;
        Options         m_Options;
    };
}
