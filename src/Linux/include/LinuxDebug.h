#pragma once

#include <asio.hpp>

#include "IDebug.h"

namespace EPRI
{
    class LinuxDebug : public IDebug
    {
    public:
        LinuxDebug() = delete;
        LinuxDebug(asio::io_service& IO);
        virtual ~LinuxDebug();
        
        virtual void TRACE(const char * Format, ...);
        virtual void TRACE_BUFFER(const char * Marker, const uint8_t * Buffer, size_t BufferSize, uint8_t BytesPerLine = 16);
        virtual void TRACE_VECTOR(const char * Marker, const DLMSVector& Data, uint8_t BytesPerLine = 16);
        
    protected:
        asio::posix::stream_descriptor    m_Output;
        asio::io_service&                 m_IO;
        
    };
    
}
