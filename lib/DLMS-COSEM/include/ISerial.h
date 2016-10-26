#pragma once 

#include <stddef.h>
#include "ERROR_TYPE.h"

namespace EPRI
{
	class ISerial
	{

	public:
		enum SerialError : uint16_t
		{
			E_SUCCESS
		};
		
		typedef struct _Options
		{
			enum BaudRate : uint8_t
			{
				BAUD_300 = 0,
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
			} m_BaudRate;
			uint8_t m_CharacterSize;
			enum Parity : uint8_t
			{
				PARITY_NONE = 0,
				PARITY_EVEN,
				PARITY_ODD
			} m_Parity;
			enum StopBits: uint8_t
			{
				STOPBITS_ONE = 0,
				STOPBITS_ONE_POINT_FIVE,
				STOPBITS_TWO
			} m_StopBits;
			
			_Options(BaudRate Baud = BAUD_300, uint8_t CharacterSize = 8, Parity Par = PARITY_NONE, StopBits StopBits = STOPBITS_ONE)
				: m_BaudRate(Baud)
				, m_CharacterSize(CharacterSize)
				, m_Parity(Par)
				, m_StopBits(StopBits)
			{
			}

		} Options;
		
		enum FlushDirection
		{
			RECEIVE = 0,
			TRANSMIT, 
			BOTH
		};

		virtual ~ISerial()
		{
		}
		virtual ERROR_TYPE Open(const char * PortName) = 0;
		virtual Options GetOptions() = 0;
		virtual ERROR_TYPE SetOptions(const Options& Opt) = 0;
		virtual ERROR_TYPE Write(const uint8_t * pBuffer, size_t Bytes) = 0;
		virtual ERROR_TYPE Read(uint8_t * pBuffer, size_t MaxBytes, uint32_t TimeOutInMS = 0, size_t * pActualBytes = nullptr) = 0;
		virtual ERROR_TYPE Close() = 0;
		virtual ERROR_TYPE Flush(FlushDirection Direction) = 0;
		virtual bool IsConnected() = 0;

	};
	
}
