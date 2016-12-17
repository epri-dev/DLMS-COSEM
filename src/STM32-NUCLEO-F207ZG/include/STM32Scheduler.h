#pragma once

#include "IScheduler.h"

namespace EPRI
{
    class STM32Scheduler : public IScheduler
    {
    public:
        STM32Scheduler();
        virtual ~STM32Scheduler();
        
        virtual void Post(PostFunction Handler);
        virtual void Sleep(uint32_t MSToSleep);

    };
	
}
