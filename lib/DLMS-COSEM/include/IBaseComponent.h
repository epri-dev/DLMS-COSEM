#pragma once

#include <stdint.h>

namespace EPRI
{
	class IBaseLibrary;

	class IBaseComponent
	{
	public:
		virtual void Initialize(IBaseLibrary * pLibrary) = 0;
		virtual IBaseLibrary * GetBase() = 0;
	};

}

