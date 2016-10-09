#include <memory>
#include <time.h>

#include "LinuxSimpleTimer.h"

namespace EPRI
{
	LinuxSimpleTimer::LinuxSimpleTimer()
		: m_State(IDLE),
		m_End(0),
		m_DurationInMilliseconds(0)
	{
	}
	
	LinuxSimpleTimer::~LinuxSimpleTimer()
	{
	}
	
	void LinuxSimpleTimer::Initialize(uint32_t DurationInMilliseconds)
	{
		m_DurationInMilliseconds = DurationInMilliseconds;
	}

	void LinuxSimpleTimer::Start()
	{
		m_State = ISimpleTimer::RUNNING;
		m_End = GetTickCount() + m_DurationInMilliseconds;
	}

	void LinuxSimpleTimer::Stop()
	{
		m_State = IDLE;
	}

	bool LinuxSimpleTimer::IsExpired()
	{
		if ((RUNNING == m_State) && (GetTickCount() >= m_End))
		{
			m_State = EXPIRED;
			return true;
		}
		return false;
	}

	uint32_t LinuxSimpleTimer::RemainingTime()
	{
		if (RUNNING == m_State)
		{
			return GetTickCount() - m_End;
		}
		return 0;
	}

	ISimpleTimer::TimerState LinuxSimpleTimer::State()
	{
		return m_State;
	}

	uint32_t LinuxSimpleTimer::GetTickCount() const
	{
		struct timespec ts;
		::clock_gettime(CLOCK_MONOTONIC, &ts);
		return (ts.tv_sec * 1000L + (ts.tv_nsec / 1000000));
	}

}

