#pragma once

#include <asio.hpp>
#include <memory>
#include "ISocket.h"

namespace EPRI
{
    class LinuxSocket : public ISocket
    {
    public:
        LinuxSocket();
        virtual ~LinuxSocket();

        virtual ERROR_TYPE Connect(const char * DestinationAddress);
        virtual ERROR_TYPE Accept();
        virtual Options GetOptions();
        virtual ERROR_TYPE SetOptions(const Options& Opt);
        virtual ERROR_TYPE Write(const uint8_t * pBuffer, size_t Bytes);
        virtual ERROR_TYPE Read(uint8_t * pBuffer, size_t MaxBytes, uint32_t TimeOutInMS , size_t * pActualBytes = nullptr);
        virtual ERROR_TYPE Close();
        virtual bool IsConnected();
        virtual bool Process();
		
    private:
        asio::io_service        m_IO;
        asio::ip::tcp::socket   m_Socket;
        Options                 m_Options;
    };
	
}