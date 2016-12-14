#include "LinuxBaseLibrary.h"
#include "DLMS-COSEM.h"

namespace EPRI
{
	LinuxBaseLibrary::LinuxBaseLibrary() :
        m_Core(m_IO), m_Debug(m_IO), m_Scheduler(m_IO)
	{
		SetBase(this);
	}
	
	LinuxBaseLibrary::~LinuxBaseLibrary()
	{
	}
	
	//
	// IBaseLibrary
	//
	IMemory * LinuxBaseLibrary::GetMemory()
	{
		return &m_Memory;
	}

	ICore * LinuxBaseLibrary::GetCore()
	{
		return &m_Core;
	}

	IScheduler * LinuxBaseLibrary::GetScheduler()
	{
		return &m_Scheduler;
	}

	ISynchronization * LinuxBaseLibrary::GetSynchronization()
	{
		return nullptr;
	}

	IDebug * LinuxBaseLibrary::GetDebug()
	{
		return &m_Debug;
	}
    
    bool LinuxBaseLibrary::Process()
    {
        return m_IO.run();
    }
	
}