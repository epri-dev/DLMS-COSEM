#include "STM32BaseLibrary.h"
#include "DLMS-COSEM.h"

namespace EPRI
{
    STM32BaseLibrary::STM32BaseLibrary()
    {
        SetBase(this);
    }
	
    STM32BaseLibrary::~STM32BaseLibrary()
    {
    }
	
    //
    // IBaseLibrary
    //
    IMemory * STM32BaseLibrary::GetMemory()
    {
        return &m_Memory;
    }

    ICore * STM32BaseLibrary::GetCore()
    {
        return &m_Core;
    }

    IScheduler * STM32BaseLibrary::GetScheduler()
    {
        return nullptr;
    }

    ISynchronization * STM32BaseLibrary::GetSynchronization()
    {
        return nullptr;
    }

    IDebug * STM32BaseLibrary::GetDebug()
    {
        return nullptr;
    }
	
}