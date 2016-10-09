#pragma once

#include "ICore.h"
#include "LinuxSerial.h"

namespace EPRI
{
	class LinuxCore : public ICore
	{
	public:
		LinuxCore();
		virtual ~LinuxCore();
		//
		// ICore
		//
		ISerial * GetSerial();
		std::shared_ptr<ISimpleTimer> CreateSimpleTimer(bool bUseHeap = true);

	private:
		LinuxSerial			m_Serial;
		
	};
	
}