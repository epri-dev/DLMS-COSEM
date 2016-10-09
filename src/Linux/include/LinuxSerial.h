#pragma once

#include <asio.hpp>
#include <memory>
#include "ISerial.h"

namespace EPRI
{
	class LinuxSerial : public ISerial
	{
	public:
		LinuxSerial();
		virtual ~LinuxSerial();

		virtual ERROR_TYPE Open(SerialPort Port);
		virtual Options GetOptions();
		virtual ERROR_TYPE SetOptions(const Options& Opt);
		virtual ERROR_TYPE Write(const uint8_t * pBuffer, size_t Bytes);
		virtual ERROR_TYPE Read(uint8_t * pBuffer, size_t MaxBytes, uint32_t TimeOutInMS = 0, size_t * pActualBytes = nullptr);
		virtual ERROR_TYPE Close();
		virtual ERROR_TYPE Flush(FlushDirection Direction);
		virtual bool IsConnected();
		
	private:
		asio::io_service  m_IO;
		asio::serial_port m_Port;
		
	};
	
}