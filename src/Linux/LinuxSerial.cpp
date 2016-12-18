#include <termios.h>
#include <chrono>
#include <iostream>
#include <iomanip>

#include "optional.h"
#include "LinuxSerial.h"
#include "IBaseLibrary.h"
#include "IDebug.h"

namespace EPRI
{
    //
    // LinuxSerial
    //
    LinuxSerial::LinuxSerial(asio::io_service& IO)
        : m_IO(IO)
    {
    }
    
    LinuxSerial::~LinuxSerial()
    {
    }
        
    ISerialSocket * LinuxSerial::CreateSocket(const ISerial::Options& Opt)
    {
        return &(*m_Sockets.emplace(m_Sockets.begin(), Opt, m_IO));
    }
    
    void LinuxSerial::ReleaseSocket(ISerialSocket * pSocket)
    {
        pSocket->Close();
        //
        // Post to allow socket cleanup befor removal.
        //
        m_IO.post(std::bind(&LinuxSerial::RemoveSocket, this, pSocket));
    }
    
    bool LinuxSerial::Process()
    {
        return true;
    }

    void LinuxSerial::RemoveSocket(ISerialSocket * pSocket)
    {
        m_Sockets.remove_if(
            [pSocket](const LinuxSerialSocket& Socket)
        {
            return &Socket == pSocket;
        });
    }    
    //
    // LinuxSerialSocket
    //
    LinuxSerialSocket::LinuxSerialSocket(const ISerial::Options& Opt, asio::io_service& IO)
        : m_Options(Opt)
        , m_Port(IO)
        , m_ReadTimer(IO)
    {
    }
    
    LinuxSerialSocket::~LinuxSerialSocket()
    {
    }

    void LinuxSerialSocket::ASIO_Write_Handler(const asio::error_code& Error, size_t BytesTransferred)
    {
        //
        // Handle Serial Disconnection
        //
        if ((asio::error::eof == Error) ||
            (asio::error::connection_reset == Error) ||
            (asio::error::operation_aborted == Error))
        {
            OnClose(SUCCESSFUL);
        }
        else if (m_Write)
        {
            m_Write(Error || !m_Port.is_open() ? !SUCCESSFUL : SUCCESSFUL, BytesTransferred);
        }
    }
    
    void LinuxSerialSocket::ASIO_Read_Handler(const asio::error_code& Error, size_t BytesTransferred)
    {
        // Port has been closed
        //
        if (asio::error::bad_descriptor == Error)
        {
            return;
        }
        // Got Data, cancel any timeouts
        //
        m_ReadTimer.cancel();
        //
        // Handle Serial Disconnection
        //
        if ((asio::error::connection_reset == Error) ||
            (asio::error::eof == Error))
        {
            OnClose(SUCCESSFUL);
        }
        else if (m_Read)
        {
            ERROR_TYPE ErrorCode = SUCCESSFUL;
            if (asio::error::operation_aborted == Error)
            {
                ErrorCode = ERR_TIMEOUT;
            }
            else if (Error)
            {
                printf("ERROR! %s\n", Error.message().c_str());
                ErrorCode = !SUCCESSFUL;
            }
            m_Read(ErrorCode, BytesTransferred);
        }
    }
    
    void LinuxSerialSocket::ASIO_Read_Timeout(const asio::error_code& Error)
    {
        if (asio::error::operation_aborted != Error)
        {
            m_Port.cancel();
        }
    }

    ERROR_TYPE LinuxSerialSocket::Open(const char * DestinationAddress /*= nullptr*/, int Port /*= DEFAULT_DLMS_PORT*/)
    {
        ERROR_TYPE RetVal = SUCCESSFUL;
        try
        {
            m_Port.open(DestinationAddress);
            SetPortOptions();
            Flush(EPRI::ISerialSocket::BOTH);
            if (m_Connect)
            {
                Base()->GetDebug()->TRACE("\n\nConnected to %s...\n\n",  
                    DestinationAddress);
                m_Connect(SUCCESSFUL);
            }
        }
        catch (asio::system_error& Error)
        {
            RetVal = !SUCCESSFUL;
        }
        return RetVal;
    }
    
    LinuxSerialSocket::ConnectCallbackFunction LinuxSerialSocket::RegisterConnectHandler(ConnectCallbackFunction Callback)
    {
        ConnectCallbackFunction RetVal = m_Connect;
        m_Connect = Callback;
        return RetVal;
    }
    
    ISerial::Options LinuxSerialSocket::GetOptions()
    {
        return m_Options;
    }
    
    ERROR_TYPE LinuxSerialSocket::Write(const DLMSVector& Data, bool Asynchronous /*= false*/)
    {
        asio::error_code SocketError;
        ERROR_TYPE       RetVal = SUCCESSFUL;

        Base()->GetDebug()->TRACE_VECTOR("SW", Data);
        
        if (Asynchronous)
        {
            if (m_Write)
            {
                asio::async_write(m_Port,
                    asio::buffer(Data.GetBytes()), 
                    std::bind(&LinuxSerialSocket::ASIO_Write_Handler, this, std::placeholders::_1, std::placeholders::_2));
            }
        }
        else
        {
            asio::write(m_Port, asio::buffer(Data.GetBytes()), asio::transfer_all(), SocketError);
            if (SocketError)
            {
                RetVal = !SUCCESSFUL;
            }
        }
        return RetVal;
    }
    
    LinuxSerialSocket::WriteCallbackFunction LinuxSerialSocket::RegisterWriteHandler(WriteCallbackFunction Callback)
    {
        WriteCallbackFunction RetVal = m_Write;
        m_Write = Callback;
        return RetVal;
    }
    
    ERROR_TYPE LinuxSerialSocket::Read(DLMSVector * pData,
        size_t ReadAtLeast /*= 0*/,
        uint32_t TimeOutInMS /*= 0*/,
        size_t * pActualBytes /*= nullptr*/)
    {
        ERROR_TYPE       RetVal = SUCCESSFUL;
        
        if (!pData /* Asynchronous */)
        {
            asio::async_read(m_Port,
                m_ReadBuffer,
                asio::transfer_exactly(ReadAtLeast ? ReadAtLeast : 1), 
                std::bind(&LinuxSerialSocket::ASIO_Read_Handler, this, std::placeholders::_1, std::placeholders::_2));
            if (TimeOutInMS)
            {
                m_ReadTimer.expires_from_now(std::chrono::milliseconds(TimeOutInMS));
                m_ReadTimer.async_wait(std::bind(&LinuxSerialSocket::ASIO_Read_Timeout,
                    this, 
                    std::placeholders::_1));
            }
            else
            {
                m_ReadTimer.cancel();
            }
        }
        else
        {
            size_t                         ActualBytes = 0;
            DLMSOptional<asio::error_code> TimerResult;
            asio::steady_timer             SynchronousTimer(m_Port.get_io_service());
            SynchronousTimer.expires_from_now(std::chrono::milliseconds(TimeOutInMS));
            
            ActualBytes = asio::read(m_Port,
                m_ReadBuffer,
                asio::transfer_exactly(ReadAtLeast ? ReadAtLeast : 1));   
            if (ActualBytes)
            {
                if (!AppendAsyncReadResult(pData, ActualBytes))
                {
                    RetVal = !SUCCESSFUL; //TODO
                }
            }
            else 
            {
                RetVal = !SUCCESSFUL;
            }
        }
        return RetVal;  
    }
    
    bool LinuxSerialSocket::AppendAsyncReadResult(DLMSVector * pData, size_t ReadAtLeast /*= 0*/)
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

        Base()->GetDebug()->TRACE_BUFFER("SR", pBuffer, ReadAtLeast);
        
        return static_cast<bool>(Stream);
    }
    
    
    LinuxSerialSocket::ReadCallbackFunction LinuxSerialSocket::RegisterReadHandler(ReadCallbackFunction Callback)
    {
        ReadCallbackFunction RetVal = m_Read;
        m_Read = Callback;
        return RetVal;
    }
    
    ERROR_TYPE LinuxSerialSocket::Close()
    {
        m_Port.cancel();
        m_Port.close();
        OnClose(SUCCESSFUL);
        return SUCCESSFUL;
    }
    
    LinuxSerialSocket::CloseCallbackFunction LinuxSerialSocket::RegisterCloseHandler(CloseCallbackFunction Callback)
    {
        CloseCallbackFunction RetVal = m_Close;
        m_Close = Callback;
        return RetVal;
    }

    bool LinuxSerialSocket::IsConnected()
    {
        return m_Port.is_open();
    }

    ERROR_TYPE LinuxSerialSocket::Flush(FlushDirection Direction)
    {
        const int FLUSHES[] = { TCIFLUSH, TCOFLUSH, TCIOFLUSH };
        ::tcflush(m_Port.lowest_layer().native_handle(), FLUSHES[Direction]);
        return SUCCESSFUL;
    }
    
    ERROR_TYPE LinuxSerialSocket::SetOptions(const ISerial::Options& Opt)
    {
        m_Options = Opt;
        return SUCCESSFUL;
    }
    
    void LinuxSerialSocket::SetPortOptions()
    {
        const uint32_t BAUDS[] = 
        {
            300,
            600,
            1200,
            1800,
            2400,
            4800,
            9600,
            19200,
            38400,
            57600,
            115200,
            230400,
            460800,
            500000,
            576000,
            921600,
            1152000,
            1500000,
            2000000,
            2500000,
            3000000,
            3500000,
            4000000
        };
        asio::serial_port_base::parity::type PARITIES[] =
        { 
            asio::serial_port_base::parity::type::none,
            asio::serial_port_base::parity::type::even,
            asio::serial_port_base::parity::type::odd
        };
        asio::serial_port_base::stop_bits::type STOPBITS[] =
        { 
            asio::serial_port_base::stop_bits::type::one,
            asio::serial_port_base::stop_bits::type::onepointfive,
            asio::serial_port_base::stop_bits::type::two
        };

        m_Port.set_option(asio::serial_port_base::baud_rate(BAUDS[m_Options.m_BaudRate]));
        m_Port.set_option(asio::serial_port_base::character_size(m_Options.m_CharacterSize));
        m_Port.set_option(asio::serial_port_base::parity(PARITIES[m_Options.m_Parity]));
        m_Port.set_option(asio::serial_port_base::stop_bits(STOPBITS[m_Options.m_StopBits]));
        m_Port.set_option(asio::serial_port_base::flow_control(asio::serial_port_base::flow_control::none));
    }   

    void LinuxSerialSocket::OnClose(ERROR_TYPE Error)
    {
        if (m_Close)
        {
            m_Close(Error);
        }
    }

}
