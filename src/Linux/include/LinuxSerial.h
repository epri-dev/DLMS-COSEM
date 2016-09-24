#pragma once

#include "asio.hpp"
#include "BaseComponent.h"
#include "ISerial.h"

namespace EPRI
{
	class LinuxSerial : public ISerial, public BaseComponent
	{
	public:
		LinuxSerial() = delete;
		LinuxSerial(IBaseLibrary * pLibrary);
		virtual ~LinuxSerial();

		virtual ERROR_TYPE Open(SerialPort Port, BaudRate Baud);
		virtual ERROR_TYPE Write(const uint8_t * pBuffer, size_t Bytes);
		virtual ERROR_TYPE Close();
		
	private:
		asio::io_service m_IO;
		asio::serial_port * m_pPort;
		
	};
	
}