#pragma once

#include "IMemory.h"
#include "BaseComponent.h"

namespace EPRI
{
	class LinuxMemory : public IMemory, public BaseComponent
	{
	public:
		LinuxMemory() = delete;
		LinuxMemory(IBaseLibrary * pLibrary);
		virtual ~LinuxMemory();
		
		virtual ERROR_TYPE Alloc(size_t Size, size_t * pActualSize, void** ppOut);
		virtual ERROR_TYPE Free(void* p);
		
	};
	
}