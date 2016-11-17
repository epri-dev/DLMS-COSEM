#include <chrono>
#include <iostream>
#include <iomanip>

#include "STM32Serial.h"

namespace EPRI
{
    STM32Serial::STM32Serial()
    {
    }
	
    STM32Serial::~STM32Serial()
    {
    }

    ERROR_TYPE STM32Serial::Open(const char * PortName)
    {
    }

    ISerial::Options STM32Serial::GetOptions()
    {
    }

    ERROR_TYPE STM32Serial::SetOptions(const ISerial::Options& Opt)
    {
    }
	

    void DUMP(const char * szMarker, const uint8_t * pBuffer, size_t BufferSize, bool AddLF = true)
    {
        if (AddLF)
            printf("%s: ", szMarker);
        const uint8_t * p = pBuffer;
        while (p != (pBuffer + BufferSize))
        {
            printf("%02X ", uint16_t(*p++));
        }
        if (AddLF)
            printf("\n");

    }
    ERROR_TYPE STM32Serial::Write(const uint8_t * pBuffer, size_t BufferSize)
    {
        DUMP("WRITE", pBuffer, BufferSize);
    }

    ERROR_TYPE STM32Serial::Read(uint8_t * pBuffer, size_t MaxBytes, uint32_t TimeOutInMS /* = 0*/, size_t * pActualBytes /*= nullptr*/)
    {
    }
	
    ERROR_TYPE STM32Serial::Flush(FlushDirection Direction)
    {
    }
	
    bool STM32Serial::IsConnected()
    {
        return false;
    }

    ERROR_TYPE STM32Serial::Close()
    {
    }

}