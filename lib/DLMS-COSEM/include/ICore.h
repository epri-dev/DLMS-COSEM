#pragma once 

#include <stddef.h>
#include "ERROR_TYPE.h"

namespace EPRI
{
	class ISerial;
	
	class ICore
	{

	public:
		enum CoreError : uint16_t
		{
			E_SUCCESS
		};

		virtual ISerial * GetSerial() = 0;

	};
	
}
