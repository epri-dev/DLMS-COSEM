#include <cstring>

#include "hdlc/HDLCStatistics.h"

namespace EPRI
{

	void HDLCStatistics::Clear()
	{
		std::memset(m_Statistics, '\0', sizeof(m_Statistics));
	}

} /* namespace EPRI */
