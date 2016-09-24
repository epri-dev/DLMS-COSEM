#pragma once

#include "Core.h"
#include "BaseComponent.h"
#include "LinuxSerial.h"

namespace EPRI
{
	class LinuxCore : public Core, public BaseComponent
	{
	public:
		LinuxCore() = delete;
		LinuxCore(IBaseLibrary * pLibrary);
		virtual ~LinuxCore();

	private:
		LinuxSerial * m_pLinuxSerial;
		
	};
	
}