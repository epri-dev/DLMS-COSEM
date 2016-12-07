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
    //
    // LinuxIP
    //
    LinuxIP::LinuxIP(asio::io_service& IO) :
        m_IO(IO)
    {
    }
    
    LinuxIP::~LinuxIP()
    {
    }
        
    ISocket * LinuxIP::CreateSocket(const IIP::Options& Opt)
    {
        return &(*m_TCPSockets.emplace(m_TCPSockets.begin(), Opt, m_IO));
    }
    
    void LinuxIP::ReleaseSocket(ISocket * pSocket)
    {
        m_TCPSockets.remove_if(
            [pSocket](const LinuxTCPSocket& Socket)
            {
                return &Socket == pSocket;
            });
    }
    
    bool LinuxIP::Process()
    {
        return true;
    }
    //
    // LinuxTCPSocket
    //
    LinuxTCPSocket::LinuxTCPSocket(const IIP::Options& Opt, asio::io_service& IO):
        m_Options(Opt), m_Acceptor(IO), m_Socket(IO), m_Resolver(IO)
    {
    }
    
    LinuxTCPSocket::~LinuxTCPSocket()
    {
    }
    
    void LinuxTCPSocket::ASIO_Accept_Handler(const asio::error_code& Error)
    {
        if (m_Connect)
        {
            Base()->GetDebug()->TRACE("\n\nConnection from %s...\n\n", 
                m_Socket.remote_endpoint().address().to_string().c_str());
            
            m_Connect(Error ? !SUCCESSFUL : SUCCESSFUL);
        }
    }
    
    void LinuxTCPSocket::ASIO_Resolver_Handler(const asio::error_code& Error, asio::ip::tcp::resolver::iterator it)
    {
        if (!Error)
        {
            tcp::endpoint Endpoint = *it;
            m_Socket.async_connect(Endpoint, 
                std::bind(&LinuxTCPSocket::ASIO_Connect_Handler, this, std::placeholders::_1, ++it));
        }
    }

    void LinuxTCPSocket::ASIO_Connect_Handler(const asio::error_code& Error, asio::ip::tcp::resolver::iterator it)
    {
        if (!Error && m_Connect)
        {
            Base()->GetDebug()->TRACE("\n\nConnected to %s...\n\n",  
                m_Socket.remote_endpoint().address().to_string().c_str());
            m_Connect(SUCCESSFUL);
        }
        else if (it != tcp::resolver::iterator())
        {
            m_Socket.close();
            tcp::endpoint Endpoint = *it;
            m_Socket.async_connect(Endpoint, 
                std::bind(&LinuxTCPSocket::ASIO_Connect_Handler, this, std::placeholders::_1, ++it));
            
        }
    }

    void LinuxTCPSocket::ASIO_Write_Handler(const asio::error_code& Error, size_t BytesTransferred)
    {
        if (m_Write)
        {
            m_Write(Error || !m_Socket.is_open() ? !SUCCESSFUL : SUCCESSFUL, BytesTransferred);
        }
    }
    
    void LinuxTCPSocket::ASIO_Read_Handler(const asio::error_code& Error, size_t BytesTransferred)
    {
        if (m_Read)
        {
            m_Read(Error ? !SUCCESSFUL : SUCCESSFUL, BytesTransferred);
        }
    }

    ERROR_TYPE LinuxTCPSocket::Open(const char * DestinationAddress /*= nullptr*/, int Port /*= DEFAULT_DLMS_PORT*/)
    {
        try
        {
            if (m_Options.m_Mode == IIP::Options::MODE_SERVER)
            {
                tcp::endpoint EndPoint = tcp::endpoint(m_Options.m_IPVersion == IIP::Options::VERSION4 ?  tcp::v4() : tcp::v6(), 
                    Port);
                m_Acceptor.open(EndPoint.protocol());
                m_Acceptor.set_option(tcp::acceptor::reuse_address(m_Options.m_ReuseAddress));
                m_Acceptor.bind(EndPoint);
                m_Acceptor.listen();
                m_Acceptor.async_accept(m_Socket, 
                    std::bind(&LinuxTCPSocket::ASIO_Accept_Handler, this, std::placeholders::_1));
            }
            else 
            {
                //
                // PRECONDITIONS
                //
                if (nullptr == DestinationAddress)
                {
                    return !SUCCESSFUL;
                }
                tcp::resolver::query    Query(DestinationAddress, std::to_string(Port));
                
                m_Socket.close();
                m_Resolver.async_resolve(Query, 
                    std::bind(&LinuxTCPSocket::ASIO_Resolver_Handler, this, std::placeholders::_1, std::placeholders::_2));
            }
		
        }
        catch (...)
        {
            return !SUCCESSFUL;
        }
        return SUCCESSFUL;
    }
    
    LinuxTCPSocket::ConnectCallbackFunction LinuxTCPSocket::RegisterConnectHandler(ConnectCallbackFunction Callback)
    {
        ConnectCallbackFunction RetVal = m_Connect;
        m_Connect = Callback;
        return RetVal;
    }
    
    IIP::Options LinuxTCPSocket::GetOptions()
    {
        return m_Options;
    }
    
    ERROR_TYPE LinuxTCPSocket::Write(const DLMSVector& Data, bool Asynchronous /*= false*/)
    {
        asio::error_code SocketError;
        ERROR_TYPE       RetVal = SUCCESSFUL;

        Base()->GetDebug()->TRACE_VECTOR("IW", Data);
        
        if (Asynchronous)
        {
            if (m_Write)
            {
                asio::async_write(m_Socket, asio::buffer(Data.GetBytes()), 
                    std::bind(&LinuxTCPSocket::ASIO_Write_Handler, this, std::placeholders::_1, std::placeholders::_2));
            }
        }
        else
        {
            asio::write(m_Socket, asio::buffer(Data.GetBytes()), asio::transfer_all(), SocketError);
            if (SocketError)
            {
                RetVal = !SUCCESSFUL;
            }
        }
        return RetVal;
    }
    
    LinuxTCPSocket::WriteCallbackFunction LinuxTCPSocket::RegisterWriteHandler(WriteCallbackFunction Callback)
    {
        WriteCallbackFunction RetVal = m_Write;
        m_Write = Callback;
        return RetVal;
    }
    
    ERROR_TYPE LinuxTCPSocket::Read(DLMSVector * pData,
        size_t ReadAtLeast /*= 0*/,
        uint32_t TimeOutInMS /*= 0*/,
        size_t * pActualBytes /*= nullptr*/)
    {
        ERROR_TYPE       RetVal = SUCCESSFUL;
        
        if (!pData /* Asynchronous */)
        {
            asio::async_read(m_Socket,
                m_ReadBuffer,
                asio::transfer_exactly(ReadAtLeast ? ReadAtLeast : 1), 
                std::bind(&LinuxTCPSocket::ASIO_Read_Handler, this, std::placeholders::_1, std::placeholders::_2));
        }
        else
        {
            size_t           ActualBytes = m_Socket.available();
            if (0 == ActualBytes)
            {
                return !SUCCESSFUL;
            }
            asio::error_code ErrorCode;
            uint8_t *        pAppend = &(*pData)[pData->AppendExtra(ActualBytes)];

            ActualBytes = m_Socket.read_some(asio::buffer(pAppend, ActualBytes), ErrorCode);
            if (ErrorCode == asio::error::eof)
            {
            }
            else if (ErrorCode)
            {
                RetVal = !SUCCESSFUL;
            }
            if (ActualBytes)
            {
                Base()->GetDebug()->TRACE_BUFFER("IR", pAppend, ActualBytes);
            }
            if (pActualBytes)
            {
                *pActualBytes = ActualBytes;
            }
        }
        return RetVal;  
    }

    bool LinuxTCPSocket::AppendAsyncReadResult(DLMSVector * pData, size_t ReadAtLeast /*= 0*/)
    {
        std::istream Stream(&m_ReadBuffer);
        if (0 == ReadAtLeast)
        {
            Stream.seekg(0, Stream.end);
            ReadAtLeast = Stream.tellg();
            Stream.seekg(0, Stream.beg);
        }
        uint8_t * pBuffer = &(*pData)[pData->AppendExtra(ReadAtLeast)];
        Stream.read((char *)pBuffer, ReadAtLeast);

        Base()->GetDebug()->TRACE_BUFFER("IR", pBuffer, ReadAtLeast);
        
        return static_cast<bool>(Stream);
    }
    
    
    LinuxTCPSocket::ReadCallbackFunction LinuxTCPSocket::RegisterReadHandler(ReadCallbackFunction Callback)
    {
        ReadCallbackFunction RetVal = m_Read;
        m_Read = Callback;
        return RetVal;
    }
    
    ERROR_TYPE LinuxTCPSocket::Close()
    {
        m_Socket.close();
        return SUCCESSFUL;
    }
    
    bool LinuxTCPSocket::IsConnected()
    {
        return m_Socket.is_open();
    }

}
