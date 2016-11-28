#pragma once

#include <cstdint>

#include "COSEMObjectInstanceID.h"

namespace EPRI
{
    typedef struct __Cosem_Attribute_Descriptor
    {
        uint16_t               class_id;
        COSEMObjectInstanceID  instance_id;
        int8_t                 attribute_id;
    }               Cosem_Attribute_Descriptor;
    typedef uint8_t InvokeIdAndPriorityType;
    
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
        
    typedef int8_t  ObjectAttributeIdType;
    typedef uint8_t ShortNameOffsetType;
}
