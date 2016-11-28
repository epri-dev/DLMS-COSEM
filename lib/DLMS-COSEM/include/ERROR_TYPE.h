///////////////////////////////////////////////////////////
//  ERROR_TYPE.h
//  Implementation of the Class ERROR_TYPE
//  Created on:      13-Sep-2016 8:18:40 PM
//  Original author: Gregory
///////////////////////////////////////////////////////////

#pragma once

#include <stdint.h>

namespace EPRI
{
	typedef uint32_t ERROR_TYPE;
	
	enum ErrorSource : uint8_t
	{
		SRC_DONT_CARE = 0,
		SRC_BASE,
		SRC_MEMORY,
		SRC_CORE,
		SRC_SCHEDULER,
		SRC_SYNCHRONIZATION,
		SRC_DEBUG,
		SRC_HDLC,
    	SRC_SOCKET,
    	SRC_SERIAL
	};

	enum ErrorLevel : uint8_t
	{
		LVL_DONT_CARE = 0,
		LVL_EMERGENCY,		// System is unusable
		LVL_ALERT,			// Action must be taken immediately
		LVL_CRITICAL,		// Critical conditions
		LVL_ERROR,			// Error conditions
		LVL_WARNING,		// Warning conditions
		LVL_NOTICE,			// Normal but significant condition
		LVL_INFORMATIONAL,	// Informational messages
		LVL_DEBUG			// Level messages
	};

	
	inline ERROR_TYPE MakeError(ErrorSource Source, ErrorLevel Level, uint16_t Code)
	{
		return (ERROR_TYPE)(Source << 24) | (Level << 16) | Code;
	}
	inline ErrorSource GetErrorSource(ERROR_TYPE Err)
	{
		return (ErrorSource)((Err & 0xFF000000) >> 24);
	}
	inline ErrorLevel GetErrorLevel(ERROR_TYPE Err)
	{
		return (ErrorLevel)((Err & 0x00FF0000) >> 16);
	}
	template <class T>
		inline T GetErrorCode(ERROR_TYPE Err)
		{
			return (T)(Err & 0x0000FFFF);
		}
	
	const ERROR_TYPE SUCCESSFUL = 0L;

}
