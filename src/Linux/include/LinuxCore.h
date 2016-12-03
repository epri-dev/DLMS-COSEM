#pragma once

#include <asio.hpp>

#include "ICore.h"
#include "LinuxSerial.h"
#include "LinuxSocket.h"

namespace EPRI
{
	class LinuxCore : public ICore
	{
	public:
    	LinuxCore() = delete;
		LinuxCore(asio::io_service& IO);
		virtual ~LinuxCore();
		//
		// ICore
		//
		ISerial * GetSerial();
    	IIP * GetIP();
    	std::shared_ptr<ISimpleTimer> CreateSimpleTimer(bool bUseHeap = true);

	private:
		LinuxSerial			m_Serial;
    	LinuxIP             m_IP;
		
	};
	
}