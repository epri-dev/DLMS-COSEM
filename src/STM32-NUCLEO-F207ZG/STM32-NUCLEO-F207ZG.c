/**
  ******************************************************************************
  * @file    FreeRTOS/FreeRTOS_ThreadCreation/Src/main.c
  * @author  MCD Application Team
  * @version V1.2.2
  * @date    25-May-2015
  * @brief   Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT(c) 2015 STMicroelectronics</center></h2>
  *
  * Licensed under MCD-ST Liberty SW License Agreement V2, (the "License");
  * You may not use this file except in compliance with the License.
  * You may obtain a copy of the License at:
  *
  *        http://www.st.com/software_license_agreement_liberty_v2
  *
  * Unless required by applicable law or agreed to in writing, software 
  * distributed under the License is distributed on an "AS IS" BASIS, 
  * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  * See the License for the specific language governing permissions and
  * limitations under the License.
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include <string.h>
#include <stm32f2xx_hal.h>
#include <../CMSIS_RTOS/cmsis_os.h>

#include "STM32-Client.h"
#include "STM32-Server.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
osThreadId DLMSThreadHandle, LEDThreadHandle;


/* Private function prototypes -----------------------------------------------*/
static void DLMSThread(void const *argument);
static void LEDThread(void const *argument);

/* Private functions ---------------------------------------------------------*/

void HAL_MspInit(void)
{
    RCC_ClkInitTypeDef RCC_ClkInitStruct;
    RCC_OscInitTypeDef RCC_OscInitStruct;

        /* Enable HSE Oscillator and activate PLL with HSE as source */
    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
    RCC_OscInitStruct.HSEState = RCC_HSE_BYPASS;
    RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
    RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
    RCC_OscInitStruct.PLL.PLLM = 8;
    RCC_OscInitStruct.PLL.PLLN = 240;
    RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
    RCC_OscInitStruct.PLL.PLLQ = 5;
    HAL_RCC_OscConfig(&RCC_OscInitStruct);
  
    /* Select PLL as system clock source and configure the HCLK, PCLK1 and PCLK2 
        clocks dividers */
    RCC_ClkInitStruct.ClockType = (RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2);
    RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
    RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
    RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
    RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;
    HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_3);
    // 
    // GPIOB Enabled for LEDs
    //
    __GPIOB_CLK_ENABLE();
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.Pin = GPIO_PIN_7 | GPIO_PIN_14;
    GPIO_InitStructure.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStructure.Speed = GPIO_SPEED_HIGH;
    GPIO_InitStructure.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStructure);
    
}

/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* User can use this section to tailor USARTx/UARTx instance used and associated
   resources */
/* Definition for USARTx clock resources */
//#define USARTx                           USART3
//#define USARTx_CLK_ENABLE()              __HAL_RCC_USART3_CLK_ENABLE();
//#define USARTx_RX_GPIO_CLK_ENABLE()      __HAL_RCC_GPIOD_CLK_ENABLE()
//#define USARTx_TX_GPIO_CLK_ENABLE()      __HAL_RCC_GPIOD_CLK_ENABLE()
//
//#define USARTx_FORCE_RESET()             __HAL_RCC_USART3_FORCE_RESET()
//#define USARTx_RELEASE_RESET()           __HAL_RCC_USART3_RELEASE_RESET()
//
///* Definition for USARTx Pins */
//#define USARTx_TX_PIN                    GPIO_PIN_8
//#define USARTx_TX_GPIO_PORT              GPIOD
//#define USARTx_TX_AF                     GPIO_AF7_USART3
//#define USARTx_RX_PIN                    GPIO_PIN_9
//#define USARTx_RX_GPIO_PORT              GPIOD
//#define USARTx_RX_AF                     GPIO_AF7_USART3

#define USARTx                           USART6
#define USARTx_CLK_ENABLE()              __HAL_RCC_USART6_CLK_ENABLE();
#define USARTx_RX_GPIO_CLK_ENABLE()      __HAL_RCC_GPIOG_CLK_ENABLE()
#define USARTx_TX_GPIO_CLK_ENABLE()      __HAL_RCC_GPIOG_CLK_ENABLE()

#define USARTx_FORCE_RESET()             __HAL_RCC_USART6_FORCE_RESET()
#define USARTx_RELEASE_RESET()           __HAL_RCC_USART6_RELEASE_RESET()

/* Definition for USARTx Pins */
#define USARTx_TX_PIN                    GPIO_PIN_14
#define USARTx_TX_GPIO_PORT              GPIOG
#define USARTx_TX_AF                     GPIO_AF8_USART6
#define USARTx_RX_PIN                    GPIO_PIN_9
#define USARTx_RX_GPIO_PORT              GPIOG
#define USARTx_RX_AF                     GPIO_AF8_USART6

void HAL_UART_MspInit(UART_HandleTypeDef *huart)
{
    //
    // USART
    //
    if (huart->Instance == USART6)
    {
        GPIO_InitTypeDef  GPIO_InitStruct;

          /*##-1- Enable peripherals and GPIO Clocks #################################*/
          /* Enable GPIO TX/RX clock */
        USARTx_TX_GPIO_CLK_ENABLE();
        USARTx_RX_GPIO_CLK_ENABLE();

          /* Enable USARTx clock */
        USARTx_CLK_ENABLE();

          /*##-2- Configure peripheral GPIO ##########################################*/
          /* UART TX GPIO pin configuration  */
        GPIO_InitStruct.Pin       = USARTx_TX_PIN;
        GPIO_InitStruct.Mode      = GPIO_MODE_AF_PP;
        GPIO_InitStruct.Pull      = GPIO_PULLUP;
        GPIO_InitStruct.Speed     = GPIO_SPEED_FREQ_VERY_HIGH;
        GPIO_InitStruct.Alternate = USARTx_TX_AF;
        HAL_GPIO_Init(USARTx_TX_GPIO_PORT, &GPIO_InitStruct);

          /* UART RX GPIO pin configuration  */
        GPIO_InitStruct.Pin = USARTx_RX_PIN;
        GPIO_InitStruct.Alternate = USARTx_RX_AF;
        HAL_GPIO_Init(USARTx_RX_GPIO_PORT, &GPIO_InitStruct);
        HAL_NVIC_SetPriority(USART6_IRQn, 14, 0);
        HAL_NVIC_EnableIRQ(USART6_IRQn);

    }
    
}

void HAL_UART_MspDeInit(UART_HandleTypeDef *huart)
{
  /*##-1- Reset peripherals ##################################################*/
    USARTx_FORCE_RESET();
    USARTx_RELEASE_RESET();

      /*##-2- Disable peripherals and GPIO Clocks #################################*/
      /* Configure UART Tx as alternate function  */
    HAL_GPIO_DeInit(USARTx_TX_GPIO_PORT, USARTx_TX_PIN);
    /* Configure UART Rx as alternate function  */
    HAL_GPIO_DeInit(USARTx_RX_GPIO_PORT, USARTx_RX_PIN);
}

/**
  * @brief  Main program
  * @param  None
  * @retval None
  */
int main(void)
{
  /* STM32F4xx HAL library initialization:
       - Configure the Flash prefetch, instruction and Data caches
       - Configure the Systick to generate an interrupt each 1 msec
       - Set NVIC Group Priority to 4
       - Global MSP (MCU Support Package) initialization
     */
	HAL_Init();  
    
    /* Thread 1 definition */
    osThreadDef(DLMSCOSEM, DLMSThread, osPriorityNormal, 0, configMINIMAL_STACK_SIZE);
  
	 /*  Thread 2 definition */
    osThreadDef(LED, LEDThread, osPriorityNormal, 0, configMINIMAL_STACK_SIZE);
  
	/* Start thread 1 */
    DLMSThreadHandle = osThreadCreate(osThread(DLMSCOSEM), NULL);
  
	/* Start thread 2 */
    LEDThreadHandle = osThreadCreate(osThread(LED), NULL);
  
	/* Start scheduler */
	osKernelStart();
    
    /* We should never get here as control is now taken by the scheduler */
	for (;;)
		;
}

void SysTick_Handler(void)
{
	osSystickHandler();
    HAL_IncTick();
}

/**
  * @brief  Run the DLMS/COSEM Server Stack
  * @param  thread not used
  * @retval None
  */
static void DLMSThread(void const *argument)
{
	(void) argument;
    
    RunServer();

}

/**
  * @brief  Toggle LED2 thread
  * @param  argument not used
  * @retval None
  */
static void LEDThread(void const *argument)
{
	uint32_t count;
	(void) argument;
  
	for (;;)
	{
		HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_14);
		osDelay(1000);
	}
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t* file, uint32_t line)
{
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

  /* Infinite loop */
	while (1)
	{
	}
}
#endif

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
