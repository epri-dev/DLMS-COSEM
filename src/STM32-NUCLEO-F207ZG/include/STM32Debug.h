#pragma once

#include "IDebug.h"

namespace EPRI
{
    class STM32Debug : public IDebug
    {
    public:
        STM32Debug();
        virtual ~STM32Debug();
        
        virtual void TRACE(const char * Format, ...);
        virtual void TRACE_BUFFER(const char * Marker, const uint8_t * Buffer, size_t BufferSize, uint8_t BytesPerLine = 16);
        virtual void TRACE_VECTOR(const char * Marker, const DLMSVector& Data, uint8_t BytesPerLine = 16);
        
    };
    
}
