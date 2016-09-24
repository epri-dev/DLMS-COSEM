#include "LinuxMemory.h"

namespace EPRI
{
	LinuxMemory::LinuxMemory(IBaseLibrary * pLibrary)
		: BaseComponent(pLibrary)
	{
	}
	
	LinuxMemory::~LinuxMemory()
	{
	}

	ERROR_TYPE LinuxMemory::Alloc(size_t Size, size_t * pActualSize, void** ppOut)
	{
		return MakeError(SRC_MEMORY, LVL_EMERGENCY, IMemory::E_SUCCESS);
	}

	ERROR_TYPE LinuxMemory::Free(void* p)
	{
		return MakeError(SRC_MEMORY, LVL_EMERGENCY, IMemory::E_SUCCESS);
	}
}