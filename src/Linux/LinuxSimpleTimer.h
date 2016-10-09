#pragma once

#include "ISimpleTimer.h"

namespace EPRI
{
	class LinuxSimpleTimer : public ISimpleTimer
	{
	public:
		LinuxSimpleTimer();
		virtual ~LinuxSimpleTimer();
		//
		// ISimpleTimer
		//
		void Initialize(uint32_t DurationInMilliseconds);
		void Start();
		void Stop();
		bool IsExpired(); 
		uint32_t RemainingTime();
		TimerState State();
		
	private:
		uint32_t GetTickCount() const;
		
		TimerState m_State;
		uint32_t   m_DurationInMilliseconds;
		uint32_t   m_End;
		
	};
	
}