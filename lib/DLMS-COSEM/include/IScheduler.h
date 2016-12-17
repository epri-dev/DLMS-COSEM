#pragma once 

#include <stddef.h>
#include <functional>

#include "ERROR_TYPE.h"

namespace EPRI
{
    class IScheduler
    {
    public:
        enum SchedulerError : uint16_t
        {
            E_SUCCESS
        };

        virtual ~IScheduler()
        {
        }
        
        typedef std::function<void(void)> PostFunction;
        virtual void Post(PostFunction Handler) = 0;
        virtual void Sleep(uint32_t MSToSleep) = 0;
    };
	
}
