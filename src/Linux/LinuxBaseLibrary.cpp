#include "LinuxBaseLibrary.h"
#include "DLMS-COSEM.h"

namespace EPRI
{
	LinuxBaseLibrary::LinuxBaseLibrary()
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
		return nullptr;
	}

	ISynchronization * LinuxBaseLibrary::GetSynchronization()
	{
		return nullptr;
	}

	IDebug * LinuxBaseLibrary::GetDebug()
	{
		return nullptr;
	}
	
}