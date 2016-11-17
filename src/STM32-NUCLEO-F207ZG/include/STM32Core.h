#pragma once

#include "ICore.h"
#include "STM32Serial.h"

namespace EPRI
{
    class STM32Core : public ICore
    {
    public:
        STM32Core();
        virtual ~STM32Core();
        //
        // ICore
        //
        ISerial * GetSerial();
        std::shared_ptr<ISimpleTimer> CreateSimpleTimer(bool bUseHeap = true);

    private:
        STM32Serial			m_Serial;
		
    };
	
}