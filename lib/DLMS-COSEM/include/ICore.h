#pragma once 

#include <stddef.h>
#include <memory>
#include "ERROR_TYPE.h"

namespace EPRI
{
	class ISerial;
	class ISimpleTimer;
	
	class ICore
	{

	public:
		enum CoreError : uint16_t
		{
			E_SUCCESS
		};
		
		virtual ~ICore()
		{
		}
		virtual ISerial * GetSerial() = 0;
		virtual std::shared_ptr<ISimpleTimer> CreateSimpleTimer(bool bUseHeap = true) = 0;

	};
	
}
