#pragma once

#include <asio.hpp>
#include <memory>
#include <list>

#include "ISocket.h"

namespace EPRI
{
    class LinuxIP;
    
    class LinuxTCPSocket : public ISocket
    {
        friend class LinuxIP;
        
    public:
        LinuxTCPSocket() = delete;
        LinuxTCPSocket(const IIP::Options& Opt, asio::io_service& IO);
        virtual ~LinuxTCPSocket();

        virtual ERROR_TYPE Open(const char * DestinationAddress = nullptr, int Port = DEFAULT_DLMS_PORT);
        virtual ConnectCallbackFunction RegisterConnectHandler(ConnectCallbackFunction Callback);
        virtual IIP::Options GetOptions();
        virtual ERROR_TYPE Write(const DLMSVector& Data, bool Asynchronous = false);
        virtual WriteCallbackFunction RegisterWriteHandler(WriteCallbackFunction Callback);
        virtual ERROR_TYPE Read(DLMSVector * pData, size_t ReadAtLeast = 0,
            uint32_t TimeOutInMS = 0,
            size_t * pActualBytes = nullptr);
        virtual bool AppendAsyncReadResult(DLMSVector * pData, size_t ReadAtLeast = 0);
        virtual ReadCallbackFunction RegisterReadHandler(ReadCallbackFunction Callback);
        virtual ERROR_TYPE Close();
        virtual bool IsConnected();
        
    private:
        void ASIO_Accept_Handler(const asio::error_code& Error);
        void ASIO_Resolver_Handler(const asio::error_code& Error, asio::ip::tcp::resolver::iterator it);
        void ASIO_Connect_Handler(const asio::error_code& Error, asio::ip::tcp::resolver::iterator it);
        void ASIO_Write_Handler(const asio::error_code& Error, size_t BytesTransferred);
        void ASIO_Read_Handler(const asio::error_code& Error, size_t BytesTransferred);

        asio::ip::tcp::resolver         m_Resolver;
        asio::ip::tcp::acceptor         m_Acceptor;
        asio::ip::tcp::socket           m_Socket;
        asio::streambuf                 m_ReadBuffer;
        IIP::Options                    m_Options;
        ConnectCallbackFunction         m_Connect;
        WriteCallbackFunction           m_Write;
        ReadCallbackFunction            m_Read;
        
    };

    class LinuxIP : public IIP
    {
        friend class LinuxTCPSocket;
        
    public:
        LinuxIP() = delete;
        LinuxIP(asio::io_service& IO);
        virtual ~LinuxIP();
        
        virtual ISocket * CreateSocket(const Options& Opt);
        virtual void ReleaseSocket(ISocket * pSocket);
        virtual bool Process();

    protected:
        using             TCPSocketList = std::list<LinuxTCPSocket>;        
        TCPSocketList     m_TCPSockets;
        asio::io_service& m_IO;
    };
	
}