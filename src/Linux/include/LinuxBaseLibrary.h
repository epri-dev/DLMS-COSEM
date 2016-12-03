#pragma once

#include <asio.hpp>

#include "IBaseLibrary.h"
#include "LinuxMemory.h"
#include "LinuxCore.h"
#include "LinuxDebug.h"

namespace EPRI
{
	class LinuxBaseLibrary : public IBaseLibrary
	{
	public:
		LinuxBaseLibrary();
		virtual ~LinuxBaseLibrary();
		//
		// IBaseLibrary
		//
		IMemory * GetMemory();
		ICore * GetCore();
		IScheduler * GetScheduler();
		ISynchronization * GetSynchronization();		
		IDebug * GetDebug();
    	bool Process();    	
    	//
    	//
    	//
    	inline asio::io_service& get_io_service()
    	{
        	return m_IO;
    	}
		
	private:
    	asio::io_service    m_IO;
		LinuxMemory	        m_Memory;
		LinuxCore           m_Core;
    	LinuxDebug          m_Debug;
	};
	
}