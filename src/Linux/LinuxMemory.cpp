#include <memory>
#include "LinuxMemory.h"

namespace EPRI
{
	LinuxMemory::LinuxMemory()
	{
	}
	
	LinuxMemory::~LinuxMemory()
	{
	}

	void * LinuxMemory::Alloc(size_t Size)
	{
		return std::calloc(Size, 1);
	}

	ERROR_TYPE LinuxMemory::Free(void* p)
	{
		if (p)
		{
			std::free(p);
		}
	}
}