#include <chrono>
#include <iostream>
#include <iomanip>
#include <../CMSIS_RTOS/cmsis_os.h>
#include <cstring>
#include <timers.h>
#include <climits>

#include "optional.h"
#include "STM32Serial.h"
#include "IBaseLibrary.h"
#include "IDebug.h"
#include "CircularBuffer.h"

extern "C"
{
    static UART_HandleTypeDef        g_Handle = {};
    static uint8_t                   g_RXBuffer[256];
    static CircularBuffer            g_Ring(g_RXBuffer, sizeof(g_RXBuffer));
    static EPRI::STM32SerialSocket * g_pSocket = nullptr;
    static TimerHandle_t             g_hRXTimer = nullptr;
    static EPRI::ERROR_TYPE          g_LastError = EPRI::SUCCESSFUL;
    static size_t                    g_BytesRead = 0;
    static osThreadId                g_CallbackThread = 0;

    void USART6_IRQHandler() 
    {
        UART_HandleTypeDef * huart = &g_Handle;
        
        uint32_t tmp1 = 0, tmp2 = 0;

        tmp1 = __HAL_UART_GET_FLAG(huart, UART_FLAG_PE);
        tmp2 = __HAL_UART_GET_IT_SOURCE(huart, UART_IT_PE);  
        /* UART parity error interrupt occurred ------------------------------------*/
        if ((tmp1 != RESET) && (tmp2 != RESET))
        { 
            __HAL_UART_CLEAR_PEFLAG(huart);
    
            huart->ErrorCode |= HAL_UART_ERROR_PE;
        }
  
        tmp1 = __HAL_UART_GET_FLAG(huart, UART_FLAG_FE);
        tmp2 = __HAL_UART_GET_IT_SOURCE(huart, UART_IT_ERR);
        /* UART frame error interrupt occurred -------------------------------------*/
        if ((tmp1 != RESET) && (tmp2 != RESET))
        { 
            __HAL_UART_CLEAR_FEFLAG(huart);
    
            huart->ErrorCode |= HAL_UART_ERROR_FE;
        }
  
        tmp1 = __HAL_UART_GET_FLAG(huart, UART_FLAG_NE);
        tmp2 = __HAL_UART_GET_IT_SOURCE(huart, UART_IT_ERR);
        /* UART noise error interrupt occurred -------------------------------------*/
        if ((tmp1 != RESET) && (tmp2 != RESET))
        { 
            __HAL_UART_CLEAR_NEFLAG(huart);
    
            huart->ErrorCode |= HAL_UART_ERROR_NE;
        }
  
        tmp1 = __HAL_UART_GET_FLAG(huart, UART_FLAG_ORE);
        tmp2 = __HAL_UART_GET_IT_SOURCE(huart, UART_IT_ERR);
        /* UART Over-Run interrupt occurred ----------------------------------------*/
        if ((tmp1 != RESET) && (tmp2 != RESET))
        { 
            __HAL_UART_CLEAR_OREFLAG(huart);
    
            huart->ErrorCode |= HAL_UART_ERROR_ORE;
        }
  
        tmp1 = __HAL_UART_GET_FLAG(huart, UART_FLAG_RXNE);
        tmp2 = __HAL_UART_GET_IT_SOURCE(huart, UART_IT_RXNE);
        /* UART in mode Receiver ---------------------------------------------------*/
        if ((tmp1 != RESET) && (tmp2 != RESET))
        { 
            uint8_t    RXByte;
            uint32_t   TmpState = 0;
            size_t     ActualBytes = 0;
            BaseType_t xHigherPriorityTaskWoken = pdFALSE;
            
            xTimerStopFromISR(g_hRXTimer, &xHigherPriorityTaskWoken);
            if (xHigherPriorityTaskWoken)
            {
                portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
            }
  
            TmpState = huart->State; 
            if ((TmpState == HAL_UART_STATE_BUSY_RX) || (TmpState == HAL_UART_STATE_BUSY_TX_RX))
            {
                if (huart->Init.Parity == UART_PARITY_NONE)
                {
                    RXByte = (huart->Instance->DR & (uint8_t)0x00FF);
                }
                else
                {
                    RXByte = (huart->Instance->DR & (uint8_t)0x007F);
                }
                g_Ring.Put(&RXByte, 1, &ActualBytes);

                if (--huart->RxXferCount == 0)
                {
                    __HAL_UART_DISABLE_IT(huart, UART_IT_RXNE);

                          /* Check if a transmit process is ongoing or not */
                    if (huart->State == HAL_UART_STATE_BUSY_TX_RX) 
                    {
                        huart->State = HAL_UART_STATE_BUSY_TX;
                    }
                    else
                    {
                      /* Disable the UART Parity Error Interrupt */
                        __HAL_UART_DISABLE_IT(huart, UART_IT_PE);

                                /* Disable the UART Error Interrupt: (Frame error, noise error, overrun error) */
                        __HAL_UART_DISABLE_IT(huart, UART_IT_ERR);

                        huart->State = HAL_UART_STATE_READY;
                    }
                    HAL_UART_RxCpltCallback(huart);
                }
            }
        }
        if (huart->ErrorCode != HAL_UART_ERROR_NONE)
        {
          /* Set the UART state ready to be able to start again the process */
            huart->State = HAL_UART_STATE_READY;
    
            HAL_UART_ErrorCallback(huart);
        }      
    }
    
    void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
    {
        g_LastError = (huart->RxXferCount == 0 ? EPRI::SUCCESSFUL : !EPRI::SUCCESSFUL);
        g_BytesRead = huart->RxXferSize - huart->RxXferCount;
        BaseType_t xHigherPriorityTaskWoken = pdFALSE;
        xTaskNotifyFromISR(g_CallbackThread,
            0x00000001,
            eSetBits,
            &xHigherPriorityTaskWoken);
        if (xHigherPriorityTaskWoken)
        {
            portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
        }
        
    }
    
    void vTimerCallback(TimerHandle_t xTimer)
    {
        UART_HandleTypeDef * huart = &g_Handle;
        
        __HAL_UART_DISABLE_IT(huart, UART_IT_RXNE);

        /* Check if a transmit process is ongoing or not */
        if (huart->State == HAL_UART_STATE_BUSY_TX_RX) 
        {
            huart->State = HAL_UART_STATE_BUSY_TX;
        }
        else
        {
          /* Disable the UART Parity Error Interrupt */
            __HAL_UART_DISABLE_IT(huart, UART_IT_PE);

            /* Disable the UART Error Interrupt: (Frame error, noise error, overrun error) */
            __HAL_UART_DISABLE_IT(huart, UART_IT_ERR);

            huart->State = HAL_UART_STATE_READY;
        }
        if (g_pSocket && g_pSocket->m_Read)
        {
            g_LastError = EPRI::ERR_TIMEOUT;
            g_BytesRead = huart->RxXferSize - huart->RxXferCount;
            BaseType_t xHigherPriorityTaskWoken = pdFALSE;
            xTaskNotify(g_CallbackThread,
                0x00000001,
                eSetBits);
            if (xHigherPriorityTaskWoken)
            {
                portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
            }

        }
    }
    
    void CallbackThread(void const * argument)
    {
        EPRI::STM32SerialSocket * pSocket = (EPRI::STM32SerialSocket *) argument;
        uint32_t                  NotifiedValue = 0;
        for (;;)
        {
            xTaskNotifyWait( 0x00, 
                ULONG_MAX,
                &NotifiedValue, 
                portMAX_DELAY);  

            if (NotifiedValue & 0x00000001 && pSocket->m_Read)
            {
                pSocket->m_Read(g_LastError, g_BytesRead);
            }
         
        }
    }
}

namespace EPRI
{
    //
    // STM32Serial
    //
    STM32Serial::STM32Serial()
    {
    }
    
    STM32Serial::~STM32Serial()
    {
    }
        
    ISerialSocket * STM32Serial::CreateSocket(const ISerial::Options& Opt)
    {
        return &(*m_Sockets.emplace(m_Sockets.begin(), Opt));
    }
    
    void STM32Serial::ReleaseSocket(ISerialSocket * pSocket)
    {
        pSocket->Close();
        //
        // Post to allow socket cleanup before removal.
        //
        //m_IO.post(std::bind(&STM32Serial::RemoveSocket, this, pSocket));
    }
    
    bool STM32Serial::Process()
    {
        return true;
    }

    void STM32Serial::RemoveSocket(ISerialSocket * pSocket)
    {
        m_Sockets.remove_if(
            [pSocket](const STM32SerialSocket& Socket)
        {
            return &Socket == pSocket;
        });
    }    
    //
    // STM32SerialSocket
    //
    STM32SerialSocket::STM32SerialSocket(const ISerial::Options& Opt)
        : m_Options(Opt)
    {
        if (g_hRXTimer)
        {
            xTimerDelete(g_hRXTimer, pdMS_TO_TICKS(100));
        }
        g_hRXTimer = xTimerCreate("RXTimer",
            pdMS_TO_TICKS(1000),
            pdFALSE,
            nullptr, 
            vTimerCallback);
 
#ifdef __GNUC__
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wwrite-strings"
#endif
        osThreadDef(Callback, CallbackThread, osPriorityNormal, 0, 20 * configMINIMAL_STACK_SIZE);
#ifdef __GNUC__
#pragma GCC diagnostic pop
#endif
        g_CallbackThread = osThreadCreate(osThread(Callback), this);
        
    }
    
    STM32SerialSocket::~STM32SerialSocket()
    {
        if (g_hRXTimer)
        {
            xTimerDelete(g_hRXTimer, pdMS_TO_TICKS(100));
        }
        g_hRXTimer = nullptr;
    }

    ERROR_TYPE STM32SerialSocket::Open(const char * DestinationAddress /*= nullptr*/, int Port /*= DEFAULT_DLMS_PORT*/)
    {
        ERROR_TYPE RetVal = SUCCESSFUL;
        SetPortOptions();
        if (HAL_UART_Init(&g_Handle) != HAL_OK)
        {
            RetVal = !SUCCESSFUL;
        }
        else
        {
            g_pSocket = this;
        }
        if (m_Connect)
        {
            m_Connect(RetVal);
        }
        return RetVal;
    }
    
    STM32SerialSocket::ConnectCallbackFunction STM32SerialSocket::RegisterConnectHandler(ConnectCallbackFunction Callback)
    {
        ConnectCallbackFunction RetVal = m_Connect;
        m_Connect = Callback;
        return RetVal;
    }
    
    ISerial::Options STM32SerialSocket::GetOptions()
    {
        return m_Options;
    }
    
    ERROR_TYPE STM32SerialSocket::Write(const DLMSVector& Data, bool Asynchronous /*= false*/)
    {
        ERROR_TYPE       RetVal = SUCCESSFUL;

        Base()->GetDebug()->TRACE_VECTOR("SW", Data);
        
        if (Asynchronous)
        {
            // TODO
        }
        else
        {
            HAL_StatusTypeDef HALVal = HAL_UART_Transmit(&g_Handle,
                                                         (uint8_t *) Data.GetData(),
                                                         (uint16_t) Data.Size(),
                                                         10000);
            if (HAL_OK != HALVal)
            {
                RetVal = !SUCCESSFUL;
            }
            
        }
        
        return RetVal;
    }
    
    STM32SerialSocket::WriteCallbackFunction STM32SerialSocket::RegisterWriteHandler(WriteCallbackFunction Callback)
    {
        WriteCallbackFunction RetVal = m_Write;
        m_Write = Callback;
        return RetVal;
    }
    
    ERROR_TYPE STM32SerialSocket::Read(DLMSVector * pData,
        size_t ReadAtLeast /*= 0*/,
        uint32_t TimeOutInMS /*= 0*/,
        size_t * pActualBytes /*= nullptr*/)
    {
        ERROR_TYPE        RetVal = SUCCESSFUL;
        HAL_StatusTypeDef HALVal;
        
        if (0 == ReadAtLeast)
        {
            ReadAtLeast = 1;
        }
        if (0 == TimeOutInMS)
        {
            TimeOutInMS = HAL_MAX_DELAY;
        }
        if (pData)
        {
            size_t BufferIndex = pData->AppendExtra(ReadAtLeast);
            HALVal = HAL_UART_Receive(&g_Handle, &(*pData)[BufferIndex], ReadAtLeast, TimeOutInMS);
            if (HAL_OK != HALVal)
            {
                RetVal = !SUCCESSFUL;
            }
            if (pActualBytes)
            {
                *pActualBytes = ReadAtLeast - g_Handle.RxXferCount;
            }

        }
        else
        {
            xTimerStop(g_hRXTimer, pdMS_TO_TICKS(10));
            if (TimeOutInMS)
            {
                xTimerChangePeriod(g_hRXTimer,
                    pdMS_TO_TICKS(TimeOutInMS),
                    pdMS_TO_TICKS(10));
                xTimerStart(g_hRXTimer, pdMS_TO_TICKS(10));
            }
            g_LastError = EPRI::SUCCESSFUL;
            HALVal = HAL_UART_Receive_IT(&g_Handle, g_RXBuffer, ReadAtLeast);
            if (HAL_OK != HALVal)
            {
                RetVal = !SUCCESSFUL;
            }
        }

        return RetVal;  
    }
    
    bool STM32SerialSocket::AppendAsyncReadResult(DLMSVector * pData, size_t ReadAtLeast /*= 0*/)
    {
        size_t RingCount;
        g_Ring.Count(&RingCount);
        
        if (0 == ReadAtLeast)
        {
            ReadAtLeast = RingCount;
        }
        if (ReadAtLeast < RingCount)
        {
            return false;
        }
        uint8_t * pBuffer = &(*pData)[pData->AppendExtra(ReadAtLeast)];
        return CircularBuffer::OK == g_Ring.Get(pBuffer, ReadAtLeast, &RingCount);
    }
    
    STM32SerialSocket::ReadCallbackFunction STM32SerialSocket::RegisterReadHandler(ReadCallbackFunction Callback)
    {
        ReadCallbackFunction RetVal = m_Read;
        m_Read = Callback;
        return RetVal;
    }
    
    ERROR_TYPE STM32SerialSocket::Close()
    {
        HAL_UART_DeInit(&g_Handle);
        std::memset(&g_Handle, '\0', sizeof(g_Handle));
        g_pSocket = nullptr;
        if (m_Close)
        {
            m_Close(SUCCESSFUL);
        }
        return SUCCESSFUL;
    }
    
    STM32SerialSocket::CloseCallbackFunction STM32SerialSocket::RegisterCloseHandler(CloseCallbackFunction Callback)
    {
        CloseCallbackFunction RetVal = m_Close;
        m_Close = Callback;
        return RetVal;
    }

    bool STM32SerialSocket::IsConnected()
    {
        return g_pSocket;
    }

    ERROR_TYPE STM32SerialSocket::Flush(FlushDirection Direction)
    {
        if (Direction == FlushDirection::BOTH || FlushDirection::RECEIVE)
        {
            g_Ring.Clear();
        }
        return SUCCESSFUL;
    }
    
    ERROR_TYPE STM32SerialSocket::SetOptions(const ISerial::Options& Opt)
    {
        m_Options = Opt;
        return SUCCESSFUL;
    }
    
    void STM32SerialSocket::SetPortOptions()
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
        uint32_t PARITIES[] = 
        { 
            UART_PARITY_NONE,
            UART_PARITY_EVEN,
            UART_PARITY_ODD
        };
        uint32_t STOPBITS[] =
        { 
            UART_STOPBITS_1,
            UART_STOPBITS_2,
            UART_STOPBITS_2
        };
        g_Handle.Instance        = USART6;
        g_Handle.Init.BaudRate   = BAUDS[m_Options.m_BaudRate];
        //
        // TODO - Word Length
        //
        g_Handle.Init.WordLength = UART_WORDLENGTH_8B;
        g_Handle.Init.StopBits   = STOPBITS[m_Options.m_StopBits];
        g_Handle.Init.Parity     = PARITIES[m_Options.m_Parity];
        g_Handle.Init.HwFlowCtl  = UART_HWCONTROL_NONE;
        g_Handle.Init.Mode       = UART_MODE_TX_RX;

    }    

}

