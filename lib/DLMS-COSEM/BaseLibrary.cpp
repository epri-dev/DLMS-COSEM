///////////////////////////////////////////////////////////
//  BaseLibrary.cpp
//  Implementation of the Class BaseLibrary
//  Created on:      13-Sep-2016 8:18:40 PM
//  Original author: Gregory
///////////////////////////////////////////////////////////

#include "BaseLibrary.h"

namespace EPRI
{
	BaseLibrary::BaseLibrary(IMemory * pMemory, ICore * pCore, IScheduler * pScheduler, ISynchronization * pSynchronization, IDebug * pDebug)
		: m_pMemory(pMemory)
		, m_pCore(pCore)
		, m_pScheduler(pScheduler)
		, m_pSynchronization(pSynchronization)
		, m_pDebug(pDebug)
	{
	}

	BaseLibrary::~BaseLibrary()
	{

	}

	IMemory * BaseLibrary::GetMemory()
	{
		return m_pMemory;
	}

	ICore * BaseLibrary::GetCore()
	{
		return m_pCore;
	}

	IScheduler * BaseLibrary::GetScheduler()
	{
		return m_pScheduler;
	}

	ISynchronization * BaseLibrary::GetSynchronization()
	{
		return m_pSynchronization;
	}

	IDebug * BaseLibrary::GetDebug()
	{
		return m_pDebug;
	}

}