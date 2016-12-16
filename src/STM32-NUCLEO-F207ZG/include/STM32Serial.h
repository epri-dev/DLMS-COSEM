#pragma once

#include <stm32f2xx_hal.h>
#include <memory>
#include <list>
#include <FreeRTOS.h>
#include <timers.h>

#include "ISerial.h"

extern "C" void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart);
extern "C" void vTimerCallback(TimerHandle_t xTimer);
extern "C" void CallbackThread(void const * argument);

namespace EPRI
{
    class STM32Serial;

    class STM32SerialSocket : public ISerialSocket
    {
        friend void ::HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart);
        friend void ::vTimerCallback(TimerHandle_t xTimer);
        friend void ::CallbackThread(void const * argument);
       
    public:
        STM32SerialSocket() = delete;
        STM32SerialSocket(const ISerial::Options& Opt);
        virtual ~STM32SerialSocket();
        
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
        void SetPortOptions();

        ISerial::Options                m_Options;
        ConnectCallbackFunction         m_Connect;
        WriteCallbackFunction           m_Write;
        ReadCallbackFunction            m_Read;
        CloseCallbackFunction           m_Close;
        
    };

    class STM32Serial : public ISerial
    {
        friend class STM32SerialSocket;
        
    public:
        STM32Serial();
        virtual ~STM32Serial();
        
        virtual ISerialSocket * CreateSocket(const Options& Opt);
        virtual void ReleaseSocket(ISerialSocket * pSocket);
        virtual bool Process();

    protected:
        void RemoveSocket(ISerialSocket * pSocket);
        
        using             SerialSocketList = std::list<STM32SerialSocket>;        
        SerialSocketList  m_Sockets;
    };    

}