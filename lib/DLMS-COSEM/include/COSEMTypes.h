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

#include <cstdint>

#include "APDU/APDUConstants.h"
#include "COSEMObjectInstanceID.h"

namespace EPRI
{
    typedef uint16_t ClassIDType;
    typedef int8_t   ObjectAttributeIdType;
    typedef uint8_t  InvokeIdAndPriorityType;
    typedef uint8_t  ShortNameOffsetType;
    
    typedef struct __Cosem_Attribute_Descriptor
    {
        ClassIDType            class_id;
        COSEMObjectInstanceID  instance_id;
        ObjectAttributeIdType  attribute_id;
    }               Cosem_Attribute_Descriptor;
    
    typedef struct __Cosem_Method_Descriptor
    {
        ClassIDType            class_id;
        COSEMObjectInstanceID  instance_id;
        ObjectAttributeIdType  method_id;
    }               Cosem_Method_Descriptor;
    
    enum COSEMPriority : uint8_t
    {
        COSEM_PRIORITY_NORMAL = 0,
        COSEM_PRIORITY_HIGH   = 0b10000000
    };
    
    enum COSEMServiceClass : uint8_t
    {
        COSEM_SERVICE_UNCONFIRMED = 0,
        COSEM_SERVICE_CONFIRMED   = 0b01000000
    };

#define COSEM_GET_INVOKE_ID(VAL)\
    (VAL & 0b00001111)
#define COSEM_GET_PRIORITY(VAL)\
    COSEMPriority(VAL & 0b10000000)
#define COSEM_GET_SERVICE_CLASS(VAL)\
    COSEMPriority(VAL & 0b01000000)

    const InvokeIdAndPriorityType ALLOWED_INVOCATION_IDS = 16;
    //
    // Get Data Result
    //
    enum Get_Data_Result_Choice : int8_t
    {
        data               = 0,
        data_access_result = 1
    };
    typedef std::variant<DLMSVector, APDUConstants::Data_Access_Result> Get_Data_Result;
#define ASN_GET_DATA_RESULT(OPTIONS)\
    ASN_BEGIN_CHOICE_WITH_OPTIONS(OPTIONS)\
        ASN_BEGIN_CHOICE_ENTRY_WITH_OPTIONS(EPRI::Get_Data_Result_Choice::data, ASN::IMPLICIT)\
            ASN_DATA_TYPE\
        ASN_END_CHOICE_ENTRY\
    ASN_END_CHOICE
    
}
