#pragma once

#include "IMemory.h"

namespace EPRI
{
	class LinuxMemory : public IMemory
	{
	public:
		LinuxMemory();
		virtual ~LinuxMemory();
		
		virtual void * Alloc(size_t Size);
		virtual ERROR_TYPE Free(void* p);
		
	};
	
}