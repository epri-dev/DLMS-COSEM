#pragma once 

#include <stddef.h>
#include "ERROR_TYPE.h"
#include "IBaseComponent.h"

namespace EPRI
{
	class ISerial
	{

	public:
		enum SerialError : uint16_t
		{
			E_SUCCESS
		};
		
		enum SerialPort : uint8_t
		{
			SERIAL_PORT_1,
			SERIAL_PORT_2,
			SERIAL_PORT_3,
			SERIAL_PORT_4,
			SERIAL_PORT_5,
			SERIAL_PORT_6,
			SERIAL_PORT_7,
			SERIAL_PORT_8
		};
		
		enum BaudRate : uint8_t
		{
			BAUD_300,
			BAUD_600,
			BAUD_1200,
			BAUD_1800,
			BAUD_2400,
			BAUD_4800,
			BAUD_9600,
			BAUD_19200,
			BAUD_38400,
			BAUD_57600,
			BAUD_115200,
			BAUD_230400,
			BAUD_460800,
			BAUD_500000,
			BAUD_576000,
			BAUD_921600,
			BAUD_1000000,
			BAUD_1152000,
			BAUD_1500000,
			BAUD_2000000,
			BAUD_2500000,
			BAUD_3000000,
			BAUD_3500000,
			BAUD_4000000
		};

		virtual ERROR_TYPE Open(SerialPort Port, BaudRate Rate = BAUD_300) = 0;
		virtual ERROR_TYPE Write(const uint8_t * pBuffer, size_t Bytes) = 0;
		virtual ERROR_TYPE Close() = 0;

	};
	
}
