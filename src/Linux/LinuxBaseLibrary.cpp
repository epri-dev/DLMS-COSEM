#include "LinuxBaseLibrary.h"

namespace EPRI
{
	LinuxBaseLibrary::LinuxBaseLibrary()
		: BaseLibrary(&m_Memory, &m_Core, NULL, NULL, NULL)
	{
	}
	
	LinuxBaseLibrary::~LinuxBaseLibrary()
	{
	}
}