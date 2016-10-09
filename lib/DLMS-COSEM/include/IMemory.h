#pragma once 

#include <stddef.h>
#include "ERROR_TYPE.h"

namespace EPRI
{
	
	class IMemory
	{

	public:
		enum MemoryError : uint16_t
		{
			E_SUCCESS,
			E_NONE_FREE
		};

		virtual ~IMemory()
		{
		}
		virtual void * Alloc(size_t Size) = 0;
		virtual ERROR_TYPE Free(void* p) = 0;

	};
	
}
