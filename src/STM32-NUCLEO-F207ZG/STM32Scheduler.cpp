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

}