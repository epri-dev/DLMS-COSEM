#pragma once

#include "IBaseLibrary.h"
#include "STM32Memory.h"
#include "STM32Core.h"

namespace EPRI
{
    class STM32BaseLibrary : public IBaseLibrary
    {
    public:
        STM32BaseLibrary();
        virtual ~STM32BaseLibrary();
        //
        // IBaseLibrary
        //
        IMemory * GetMemory();
        ICore * GetCore();
        IScheduler * GetScheduler();
        ISynchronization * GetSynchronization();		
        IDebug * GetDebug();
		
    private:
        STM32Memory	 m_Memory;
        STM32Core    m_Core;
		
    };
	
}