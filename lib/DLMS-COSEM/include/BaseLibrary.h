#pragma once

#include "IBaseLibrary.h"
#include "IMemory.h"
#include "ICore.h"

namespace EPRI
{

	class BaseLibrary : public IBaseLibrary
	{
	public:
		virtual ~BaseLibrary();

		virtual IMemory * GetMemory();
		virtual ICore * GetCore();
		virtual IScheduler * GetScheduler();
		virtual ISynchronization * GetSynchronization();
		virtual IDebug * GetDebug();

	protected:
		BaseLibrary(IMemory * pMemory, ICore * pCore, IScheduler * pScheduler, ISynchronization * pSynchronization, IDebug * pDebug);
		
	private:
		ICore* m_pCore;
		IMemory* m_pMemory;
		IScheduler* m_pScheduler;
		ISynchronization* m_pSynchronization;
		IDebug* m_pDebug;

	};

}
