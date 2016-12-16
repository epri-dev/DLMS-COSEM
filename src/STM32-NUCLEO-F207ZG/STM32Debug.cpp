#include <cstdio>
#include <cstdarg>

#include "STM32Debug.h"

namespace EPRI
{
    STM32Debug::STM32Debug()
    {
    }
    
    STM32Debug::~STM32Debug()
    {
    }
    
    void STM32Debug::TRACE(const char * Format, ...)
    {
        va_list Args;
        va_start(Args, Format);
#ifdef DEBUG
        //vprintf(Format, Args);
#endif
        va_end(Args);
    }
    
    void STM32Debug::TRACE_BUFFER(const char * Marker, const uint8_t * Buffer, size_t BufferSize, uint8_t BytesPerLine /*= 16*/)
    {
        TRACE("\n%s: ", Marker);
        const uint8_t * p = Buffer;
        while (p != (Buffer + BufferSize))
        {
            TRACE("%02X ", uint16_t(*p++));
            if (0 == (size_t(p - Buffer) % BytesPerLine))
            {
                TRACE("\n%s: ", Marker);
            }
        }
        TRACE("\n");
    }
    
    void STM32Debug::TRACE_VECTOR(const char * Marker, const DLMSVector& Data, uint8_t BytesPerLine /*= 16*/)
    {
        TRACE_BUFFER(Marker, Data.GetData(), Data.Size(), BytesPerLine);
    }

    
}