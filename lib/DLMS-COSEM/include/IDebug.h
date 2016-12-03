#pragma once

#include <cstdint>
#include <cstddef>

#include "DLMSVector.h"

namespace EPRI
{
    class IDebug
    {
    public:
        virtual ~IDebug()
        {
        }
        virtual void TRACE(const char * Format, ...) = 0;
        virtual void TRACE_BUFFER(const char * Marker, const uint8_t * Buffer, size_t BufferSize, uint8_t BytesPerLine = 16) = 0;
        virtual void TRACE_VECTOR(const char * Marker, const DLMSVector& Data, uint8_t BytesPerLine = 16) = 0;
    };
}
