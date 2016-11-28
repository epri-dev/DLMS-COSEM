#include <termios.h>
#include <chrono>
#include <iostream>
#include <iomanip>
#include <string>

#include "optional.h"
#include "LinuxSocket.h"
#include "IBaseLibrary.h"
#include "IDebug.h"

namespace EPRI
{
    using namespace asio::ip;
    
    LinuxSocket::LinuxSocket() :
        m_Socket(m_IO)
    {
    }
    
    LinuxSocket::~LinuxSocket()
    {
    }
    
    ERROR_TYPE LinuxSocket::Connect(const char * DestinationAddress)
    {
        tcp::resolver           Resolver(m_IO);
        tcp::resolver::query    Query(DestinationAddress, std::to_string(m_Options.m_Port));
        tcp::resolver::iterator EndpointIT = Resolver.resolve(Query);
        tcp::resolver::iterator End;
        asio::error_code        ErrorCode = asio::error::host_not_found;
        while (ErrorCode && EndpointIT != End)
        {
            m_Socket.close();
            m_Socket.connect(*EndpointIT++, ErrorCode);
        }
        if (ErrorCode)
        {
            return !SUCCESSFUL;
        }
        return SUCCESSFUL;        
    }
    
    ERROR_TYPE LinuxSocket::Accept()
    {
        try
        {
            tcp::acceptor Acceptor(m_IO,
                tcp::endpoint(m_Options.m_IPVersion == EPRI::ISocket::_Options::VERSION4 ? tcp::v4() : tcp::v6(),
                    m_Options.m_Port));
            Acceptor.accept(m_Socket);
        }
        catch (const std::exception&)
        {
            return !SUCCESSFUL;
        }
        return SUCCESSFUL;
    }
    
    LinuxSocket::Options LinuxSocket::GetOptions()
    {
        return m_Options;
    }
    
    ERROR_TYPE LinuxSocket::SetOptions(const Options& Opt)
    {
        m_Options = Opt;
    }
    
    ERROR_TYPE LinuxSocket::Write(const uint8_t * pBuffer, size_t Bytes)
    {
        asio::error_code SocketError;
        Base()->GetDebug()->TRACE_BUFFER("Socket Write", pBuffer, Bytes);
        asio::write(m_Socket, asio::buffer(pBuffer, Bytes), asio::transfer_all(), SocketError);
        return SUCCESSFUL;
    }
    
    ERROR_TYPE LinuxSocket::Read(uint8_t * pBuffer, size_t MaxBytes, uint32_t TimeOutInMS, size_t * pActualBytes /* = nullptr */)
    {
        ERROR_TYPE       RetVal = SUCCESSFUL;
        size_t           ActualBytes = 0;
        asio::error_code ErrorCode;
        if (!m_Socket.available())
        {
            return !SUCCESSFUL;
        }
        ActualBytes = m_Socket.read_some(asio::buffer(pBuffer, MaxBytes), ErrorCode);
        if (ErrorCode == asio::error::eof)
        {
        }
        else if (ErrorCode)
        {
            RetVal = !SUCCESSFUL;
        }
        if (ActualBytes)
        {
            Base()->GetDebug()->TRACE_BUFFER("Socket Read", pBuffer, ActualBytes);
        }
        if (pActualBytes)
        {
            *pActualBytes = ActualBytes;
        }
        return RetVal;  
    }
    
    ERROR_TYPE LinuxSocket::Close()
    {
        m_Socket.close();
        return SUCCESSFUL;
    }
    
    bool LinuxSocket::IsConnected()
    {
        return m_Socket.is_open();
    }
    
    bool LinuxSocket::Process()
    {
        return true;
    }

}