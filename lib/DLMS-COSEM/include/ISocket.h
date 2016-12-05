#pragma once

#include <cstdint>
#include <cstddef>
#include <functional>

#include "ERROR_TYPE.h"
#include "Callback.h"
#include "DLMSVector.h"

namespace EPRI
{
    class ISocket;
    
    class IIP
    {
    public:
        virtual ~IIP()
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
            enum Protocol : uint8_t
            {
                TCP,
                UDP
            }                   m_Protocol = TCP;
            bool                m_ReuseAddress = true;
            
            _Options(SocketMode Mode = MODE_SERVER,
                IPVersion Version = VERSION4, 
                Protocol Prot = TCP,
                bool ReuseAddress = true) : 
                m_Mode(Mode), 
                m_IPVersion(Version), 
                m_Protocol(Prot),
                m_ReuseAddress(ReuseAddress)
            {
            }
            
        } Options;

        virtual ISocket * CreateSocket(const Options& Opt) = 0;
        virtual void ReleaseSocket(ISocket * pSocket) = 0;
        virtual bool Process() = 0;
        
    };
    
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
        
        typedef std::function<void(ERROR_TYPE)>         ConnectCallbackFunction;
        typedef std::function<void(ERROR_TYPE, size_t)> WriteCallbackFunction;
        typedef std::function<void(ERROR_TYPE, size_t)> ReadCallbackFunction;
        typedef std::function<void(ERROR_TYPE)>         CloseCallbackFunction;

        virtual ERROR_TYPE Open(const char * DestinationAddress = nullptr, int Port = DEFAULT_DLMS_PORT) = 0;
        virtual ConnectCallbackFunction RegisterConnectHandler(ConnectCallbackFunction Callback) = 0;
        virtual IIP::Options GetOptions() = 0;
        virtual ERROR_TYPE Write(const DLMSVector& Data, bool Asynchronous = false) = 0;
        virtual WriteCallbackFunction RegisterWriteHandler(WriteCallbackFunction Callback) = 0;
        virtual ERROR_TYPE Read(DLMSVector * pData, size_t ReadAtLeast = 0,
                                uint32_t TimeOutInMS = 0, size_t * pActualBytes = nullptr) = 0;
        virtual bool AppendAsyncReadResult(DLMSVector * pData, size_t ReadAtLeast = 0) = 0;
        virtual ReadCallbackFunction RegisterReadHandler(ReadCallbackFunction Callback) = 0;
        virtual ERROR_TYPE Close() = 0;
        virtual CloseCallbackFunction RegisterCloseHandler(CloseCallbackFunction Callback) = 0;
        virtual bool IsConnected() = 0;
        
    };
    
}