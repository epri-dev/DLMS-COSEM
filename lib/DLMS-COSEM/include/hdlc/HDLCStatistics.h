
#pragma once

#include <cstdint>

namespace EPRI
{

	class HDLCStatistics
	{
	public:
		enum Statistic
		{
			PACKET_CRC                         = 0,
			PACKET_TOO_LONG,
			PACKET_TOO_SHORT,
			PACKET_TIMEOUT,
			PACKET_TRANSMIT_FAILURE,
			PACKET_RECEIVE_FAILURE,
			PACKET_WRONG_ADDRESS,
			PACKET_INVALID_COMMAND,
			DATALINK_RESETS,
			DATALINK_TRANSMIT_OVERRUN,
			DATALINK_TRANSMIT_BUFFER_TOO_LONG,
			DATALINK_RECEIVE_PACKET_ORDER,
			DATALINK_TRANSMIT_TIMEOUT,
			DATALINK_TRANSMIT_BUFFER_TOO_SHORT,
			DATALINK_TRANSMIT_PACKET_OVERFLOW,
			DATALINK_RECEIVE_FAILURE,
			DATALINK_TRANSMIT_RETRY,
			MAX_STATISTICS
		};
		inline uint32_t Get(Statistic Stat)
		{
			return m_Statistics[Stat];
		}

		inline uint32_t Increment(Statistic Stat)
		{
			return ++m_Statistics[Stat];
		}

		void Clear();

	private:
    	uint32_t m_Statistics[MAX_STATISTICS] = { };

	};

} /* namespace EPRI */
