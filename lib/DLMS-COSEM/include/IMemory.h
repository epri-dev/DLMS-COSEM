///////////////////////////////////////////////////////////
//  IMemory.h
//  Implementation of the Interface IMemory
//  Created on:      13-Sep-2016 8:18:40 PM
//  Original author: Gregory
///////////////////////////////////////////////////////////

#pragma once 

#include <stddef.h>
#include "ERROR_TYPE.h"
#include "IBaseComponent.h"

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

		virtual ERROR_TYPE Alloc(size_t Size, size_t * pActualSize, void** ppOut) = 0;
		virtual ERROR_TYPE Free(void* p) = 0;

	};
	
}
