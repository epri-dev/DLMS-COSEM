#include <cstdio>
#include <cstdarg>

#include "LinuxDebug.h"

namespace EPRI
{
    LinuxDebug::LinuxDebug(asio::io_service& IO) :
        m_Output(IO, ::dup(STDOUT_FILENO)), m_IO(IO)
    {
    }
    
    LinuxDebug::~LinuxDebug()
    {
    }
    
    void LinuxDebug::TRACE(const char * Format, ...)
    {
        char Buffer[256];
        va_list Args;
        va_start(Args, Format);
        vsnprintf(Buffer, sizeof(Buffer), Format, Args);
        asio::write(m_Output, asio::buffer(Buffer, std::strlen(Buffer)));
        va_end(Args);
    }
    
    void LinuxDebug::TRACE_BUFFER(const char * Marker, const uint8_t * Buffer, size_t BufferSize, uint8_t BytesPerLine /*= 16*/)
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
    
    void LinuxDebug::TRACE_VECTOR(const char * Marker, const DLMSVector& Data, uint8_t BytesPerLine /*= 16*/)
    {
        TRACE_BUFFER(Marker, Data.GetData(), Data.Size(), BytesPerLine);
    }

    
}