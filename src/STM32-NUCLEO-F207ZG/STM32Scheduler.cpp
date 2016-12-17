#include <FreeRTOS.h>
#include <task.h>

#include "STM32Scheduler.h"

namespace EPRI
{
    STM32Scheduler::STM32Scheduler()
    {
    }
    
    STM32Scheduler::~STM32Scheduler()
    {
    }
        
    void STM32Scheduler::Post(PostFunction Handler)
    {
        Handler();
    }

    void STM32Scheduler::Sleep(uint32_t MSToSleep)
    {
        vTaskDelay(pdMS_TO_TICKS(MSToSleep));
    }

}