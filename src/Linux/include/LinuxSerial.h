#pragma once

#include <asio.hpp>
#include <memory>
#include <list>

#include "ISerial.h"

namespace EPRI
{
    class LinuxSerial;
    
    class LinuxSerialSocket : public ISerialSocket
    {
        friend class LinuxIP;
        
    public:
        LinuxSerialSocket() = delete;
        LinuxSerialSocket(const ISerial::Options& Opt, asio::io_service& IO);
        virtual ~LinuxSerialSocket();
        
        ISerial::Options GetOptions();
        //
        // ISocket
        //
        virtual ERROR_TYPE Open(const char * DestinationAddress = nullptr, int Port = DEFAULT_DLMS_PORT);
        virtual ConnectCallbackFunction RegisterConnectHandler(ConnectCallbackFunction Callback);
        virtual ERROR_TYPE Write(const DLMSVector& Data, bool Asynchronous = false);
        virtual WriteCallbackFunction RegisterWriteHandler(WriteCallbackFunction Callback);
        virtual ERROR_TYPE Read(DLMSVector * pData,
            size_t ReadAtLeast = 0,
            uint32_t TimeOutInMS = 0,
            size_t * pActualBytes = nullptr);
        virtual bool AppendAsyncReadResult(DLMSVector * pData, size_t ReadAtLeast = 0);
        virtual ReadCallbackFunction RegisterReadHandler(ReadCallbackFunction Callback);
        virtual ERROR_TYPE Close();
        virtual CloseCallbackFunction RegisterCloseHandler(CloseCallbackFunction Callback);
        virtual bool IsConnected();
        //
        // ISerialSocket
        //
        virtual ERROR_TYPE Flush(FlushDirection Direction);
        virtual ERROR_TYPE SetOptions(const ISerial::Options& Opt);
        
    private:
        void ASIO_Write_Handler(const asio::error_code& Error, size_t BytesTransferred);
        void ASIO_Read_Handler(const asio::error_code& Error, size_t BytesTransferred);
        void ASIO_Read_Timeout(const asio::error_code& Error);
        void SetPortOptions();
        
        void OnClose(ERROR_TYPE Error);

        asio::serial_port               m_Port;
        asio::steady_timer              m_ReadTimer;
        asio::streambuf                 m_ReadBuffer;
        ISerial::Options                m_Options;
        ConnectCallbackFunction         m_Connect;
        WriteCallbackFunction           m_Write;
        ReadCallbackFunction            m_Read;
        CloseCallbackFunction           m_Close;
    };

    class LinuxSerial : public ISerial
    {
        friend class LinuxSerialSocket;
        
    public:
        LinuxSerial() = delete;
        LinuxSerial(asio::io_service& IO);
        virtual ~LinuxSerial();
        
        virtual ISerialSocket * CreateSocket(const Options& Opt);
        virtual void ReleaseSocket(ISerialSocket * pSocket);
        virtual bool Process();

    protected:
        void RemoveSocket(ISerialSocket * pSocket);
        
        using             SerialSocketList = std::list<LinuxSerialSocket>;        
        SerialSocketList  m_Sockets;
        asio::io_service& m_IO;
    };    

}