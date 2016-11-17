#include "STM32Core.h"
#include "STM32SimpleTimer.h"

namespace EPRI
{
    STM32Core::STM32Core()
    {
    }
	
    STM32Core::~STM32Core()
    {
    }

    ISerial * STM32Core::GetSerial()
    {
        return &m_Serial;
    }
	
    std::shared_ptr<ISimpleTimer> STM32Core::CreateSimpleTimer(bool bUseHeap /* = true*/)
    {
    	// TODO - Embedded memory management
		
        return std::shared_ptr<ISimpleTimer>(new STM32SimpleTimer);
    }

}