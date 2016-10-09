#pragma once

#include "IBaseLibrary.h"
#include "LinuxMemory.h"
#include "LinuxCore.h"

namespace EPRI
{
	class LinuxBaseLibrary : public IBaseLibrary
	{
	public:
		LinuxBaseLibrary();
		virtual ~LinuxBaseLibrary();
		//
		// IBaseLibrary
		//
		IMemory * GetMemory();
		ICore * GetCore();
		IScheduler * GetScheduler();
		ISynchronization * GetSynchronization();		
		IDebug * GetDebug();
		
	private:
		LinuxMemory	 m_Memory;
		LinuxCore    m_Core;
		
	};
	
}