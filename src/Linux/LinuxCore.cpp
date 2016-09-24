#include "LinuxCore.h"

namespace EPRI
{
	LinuxCore::LinuxCore(IBaseLibrary * pLibrary) :
		m_pLinuxSerial(new LinuxSerial(pLibrary)),
		BaseComponent(pLibrary)
	{
		SetSerial(m_pLinuxSerial);
	}
	
	LinuxCore::~LinuxCore()
	{
	}

}