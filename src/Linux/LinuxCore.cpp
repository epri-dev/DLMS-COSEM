#include "LinuxCore.h"
#include "LinuxSimpleTimer.h"

namespace EPRI
{
	LinuxCore::LinuxCore()
	{
	}
	
	LinuxCore::~LinuxCore()
	{
	}

	ISerial * LinuxCore::GetSerial()
	{
		return &m_Serial;
	}
	
	std::shared_ptr<ISimpleTimer> LinuxCore::CreateSimpleTimer(bool bUseHeap /* = true*/)
	{
		// TODO - Embedded memory management
		
		return std::shared_ptr<ISimpleTimer>(new LinuxSimpleTimer);
	}

}