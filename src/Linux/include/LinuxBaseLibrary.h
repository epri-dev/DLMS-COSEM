#pragma once

#include "BaseLibrary.h"
#include "LinuxMemory.h"
#include "LinuxCore.h"

namespace EPRI
{
	class LinuxBaseLibrary : public BaseLibrary
	{
	public:
		LinuxBaseLibrary();
		virtual ~LinuxBaseLibrary();
		
	private:
		LinuxMemory m_Memory = LinuxMemory(this);
		LinuxCore m_Core = LinuxCore(this);
		
	};
	
}