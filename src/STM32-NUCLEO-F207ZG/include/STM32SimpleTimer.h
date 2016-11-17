#pragma once

#include "ISimpleTimer.h"

namespace EPRI
{
    class STM32SimpleTimer : public ISimpleTimer
    {
    public:
        STM32SimpleTimer();
        virtual ~STM32SimpleTimer();
        //
        // ISimpleTimer
        //
        void Initialize(uint32_t DurationInMilliseconds);
        void Start();
        void Stop();
        bool IsExpired(); 
        uint32_t RemainingTime();
        TimerState State();
		
    };
	
}