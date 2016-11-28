#pragma once

#include "IDebug.h"

namespace EPRI
{
    class LinuxDebug : public IDebug
    {
    public:
        LinuxDebug();
        virtual ~LinuxDebug();
        virtual void TRACE(const char * Format, ...);
        virtual void TRACE_BUFFER(const char * Marker, const uint8_t * Buffer, size_t BufferSize, uint8_t BytesPerLine = 16);
    };
    
}
