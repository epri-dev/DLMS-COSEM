#include "Core.h"

namespace EPRI
{
	Core::Core() :
		m_pSerial(NULL)
	{
	}
	
	Core::Core(ISerial * Serial) : 
		m_pSerial(Serial)
	{
	}
		
	Core::~Core()
	{
	}

	ISerial * Core::GetSerial()
	{
		return m_pSerial;
	}

	void Core::SetSerial(ISerial * pSerial)
	{
		 m_pSerial = pSerial;
	}
	
}