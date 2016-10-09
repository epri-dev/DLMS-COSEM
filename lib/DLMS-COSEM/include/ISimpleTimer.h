#pragma once 

#include <cstdint>
#include <memory>

namespace EPRI
{
	
	class ISimpleTimer
	{

	public:
		enum TimerState : uint8_t
		{
			IDLE,
			EXPIRED,
			RUNNING
		};
		virtual ~ISimpleTimer() 
		{
		}
		virtual void Initialize(uint32_t DurationInMilliseconds) = 0;
		virtual void Start() = 0;
		virtual void Stop() = 0;
		virtual bool IsExpired() = 0; 
		virtual uint32_t RemainingTime() = 0;
		virtual TimerState State() = 0;

	};
	
}
