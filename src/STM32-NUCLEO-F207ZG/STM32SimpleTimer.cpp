#include <memory>
#include <time.h>

#include "STM32SimpleTimer.h"

namespace EPRI
{
    STM32SimpleTimer::STM32SimpleTimer()
    {
    }
	
    STM32SimpleTimer::~STM32SimpleTimer()
    {
    }
	
    void STM32SimpleTimer::Initialize(uint32_t DurationInMilliseconds)
    {
    }

    void STM32SimpleTimer::Start()
    {
    }

    void STM32SimpleTimer::Stop()
    {
    }

    bool STM32SimpleTimer::IsExpired()
    {
        return false;
    }

    uint32_t STM32SimpleTimer::RemainingTime()
    {
        return 0;
    }

    ISimpleTimer::TimerState STM32SimpleTimer::State()
    {
    }

}

