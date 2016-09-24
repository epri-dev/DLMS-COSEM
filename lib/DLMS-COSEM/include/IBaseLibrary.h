#pragma once

#include <stdint.h>
#include "ERROR_TYPE.h"

namespace EPRI
{

	class IMemory;
	class ICore;
	class IScheduler;
	class ISynchronization;
	class IDebug;

	class IBaseLibrary
	{

	public:
		virtual IMemory * GetMemory() = 0;
		virtual ICore * GetCore() = 0;
		virtual IScheduler * GetScheduler() = 0;
		virtual ISynchronization * GetSynchronization() = 0;		
		virtual IDebug * GetDebug() = 0;

	};

}

