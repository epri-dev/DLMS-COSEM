#pragma once 

#include <stddef.h>
#include <memory>

#include "ERROR_TYPE.h"

namespace EPRI
{
    class ISemaphore
    {
    public:
        virtual ~ISemaphore()
        {
        }
        virtual bool Take() = 0;
        virtual bool Give() = 0;
    };
    
    typedef std::unique_ptr<ISemaphore> ISemaphorePtr;
    
    class ISynchronization
    {
    public:
        virtual ~ISynchronization()
        {
        }
        virtual ISemaphorePtr CreateSemaphore() = 0;
        
    };
    
    
	
}
