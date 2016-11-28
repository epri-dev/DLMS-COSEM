#pragma once

#include <cstdint>
#include <cstddef>

#include "ERROR_TYPE.h"
#include "Callback.h"

namespace EPRI
{
    class ISocket
    {
    public:
        static const int DEFAULT_DLMS_PORT = 4059;
        
        enum SocketError : uint16_t
        {
            E_SUCCESS
        };
        
        virtual ~ISocket()
        {
        }

        typedef struct _Options
        {
            enum SocketMode : uint8_t
            {
                MODE_SERVER,
                MODE_CLIENT
            }                   m_Mode = MODE_SERVER;
            enum IPVersion : uint8_t
            {
                VERSION4 = 4,
                VERSION6 = 6
            }                   m_IPVersion = VERSION4;
            int                 m_Port = DEFAULT_DLMS_PORT;
            
            _Options(SocketMode Mode = MODE_SERVER, IPVersion Version = VERSION4, int Port = DEFAULT_DLMS_PORT):
                m_IPVersion(Version), m_Port(Port)
            {
            }
            
        } Options;
        
        virtual ERROR_TYPE Connect(const char * DestinationAddress) = 0;
        virtual ERROR_TYPE Accept() = 0;
        virtual Options GetOptions() = 0;
        virtual ERROR_TYPE SetOptions(const Options& Opt) = 0;
        virtual ERROR_TYPE Write(const uint8_t * pBuffer, size_t Bytes) = 0;
        virtual ERROR_TYPE Read(uint8_t * pBuffer, size_t MaxBytes, uint32_t TimeOutInMS = 0, size_t * pActualBytes = nullptr) = 0;
        virtual ERROR_TYPE Close() = 0;
        virtual bool IsConnected() = 0;
        virtual bool Process() = 0;
        
    };
    
}