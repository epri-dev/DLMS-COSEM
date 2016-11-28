#include <cstdio>
#include <cstdarg>

#include "LinuxDebug.h"

namespace EPRI
{
    LinuxDebug::LinuxDebug()
    {
    }
    
    LinuxDebug::~LinuxDebug()
    {
    }
    
    void LinuxDebug::TRACE(const char * Format, ...)
    {
        va_list Args;
        va_start(Args, Format);
        vprintf(Format, Args);
        va_end(Args);
    }
    
    void LinuxDebug::TRACE_BUFFER(const char * Marker, const uint8_t * Buffer, size_t BufferSize, uint8_t BytesPerLine /*= 16*/)
    {
        std::printf("%s________________\n", Marker);
        const uint8_t * p = Buffer;
        while (p != (Buffer + BufferSize))
        {
            std::printf("%02X ", uint16_t(*p++));
            if (0 == (size_t(p - Buffer) % BytesPerLine))
            {
                std::printf("\n");
            }
        }
        std::printf("\n");
    }
    
}