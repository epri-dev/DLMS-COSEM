#pragma once

#include "IMemory.h"

namespace EPRI
{
    class STM32Memory : public IMemory
    {
    public:
        STM32Memory();
        virtual ~STM32Memory();
		
        virtual void * Alloc(size_t Size);
        virtual ERROR_TYPE Free(void* p);
		
    };
	
}