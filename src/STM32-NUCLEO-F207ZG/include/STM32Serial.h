#pragma once

#include <memory>
#include "ISerial.h"

namespace EPRI
{
    class STM32Serial : public ISerial
    {
    public:
        STM32Serial();
        virtual ~STM32Serial();

        virtual ERROR_TYPE Open(const char * PortName);
        virtual Options GetOptions();
        virtual ERROR_TYPE SetOptions(const Options& Opt);
        virtual ERROR_TYPE Write(const uint8_t * pBuffer, size_t Bytes);
        virtual ERROR_TYPE Read(uint8_t * pBuffer, size_t MaxBytes, uint32_t TimeOutInMS = 0, size_t * pActualBytes = nullptr);
        virtual ERROR_TYPE Close();
        virtual ERROR_TYPE Flush(FlushDirection Direction);
        virtual bool IsConnected();
		
    };
	
}