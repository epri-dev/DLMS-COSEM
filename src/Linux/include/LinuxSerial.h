#pragma once

#include <asio.hpp>
#include <memory>

#include "ISerial.h"

namespace EPRI
{
	class LinuxSerial : public ISerial
	{
	public:
		LinuxSerial() = delete;
    	LinuxSerial(asio::io_service& IO);
    	virtual ~LinuxSerial();

		virtual ERROR_TYPE Open(const char * PortName);
		virtual Options GetOptions();
		virtual ERROR_TYPE SetOptions(const Options& Opt);
		virtual ERROR_TYPE Write(const uint8_t * pBuffer, size_t Bytes);
		virtual ERROR_TYPE Read(uint8_t * pBuffer, size_t MaxBytes, uint32_t TimeOutInMS = 0, size_t * pActualBytes = nullptr);
		virtual ERROR_TYPE Close();
		virtual ERROR_TYPE Flush(FlushDirection Direction);
		virtual bool IsConnected();
		
	private:
    	void SetPortOptions();
    	
		asio::serial_port m_Port;
    	Options           m_Options;
		
	};
	
}