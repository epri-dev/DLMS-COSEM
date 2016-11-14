#include <termios.h>
#include <chrono>
#include <iostream>
#include <iomanip>

#include "optional.h"
#include "LinuxSerial.h"

namespace EPRI
{
	LinuxSerial::LinuxSerial()
		: m_Port(m_IO)
	{
	}
	
	LinuxSerial::~LinuxSerial()
	{
	}

	ERROR_TYPE LinuxSerial::Open(const char * PortName)
	{
    	m_Port.open(PortName);
		return SetOptions(Options());
	}

	ISerial::Options LinuxSerial::GetOptions()
	{
	}

    ERROR_TYPE LinuxSerial::SetOptions(const ISerial::Options& Opt)
    {
        const uint32_t BAUDS[] = 
        {
            300,
            600,
            1200,
            1800,
            2400,
            4800,
            9600,
            19200,
            38400,
            57600,
            115200,
            230400,
            460800,
            500000,
            576000,
            921600,
            1152000,
            1500000,
            2000000,
            2500000,
            3000000,
            3500000,
            4000000
        };
        asio::serial_port_base::parity::type PARITIES[] =
        { 
            asio::serial_port_base::parity::type::none,
            asio::serial_port_base::parity::type::even,
            asio::serial_port_base::parity::type::odd
        };
        asio::serial_port_base::stop_bits::type STOPBITS[] =
        { 
            asio::serial_port_base::stop_bits::type::one,
            asio::serial_port_base::stop_bits::type::onepointfive,
            asio::serial_port_base::stop_bits::type::two
        };

        m_Port.set_option(asio::serial_port_base::baud_rate(BAUDS[Opt.m_BaudRate]));
        m_Port.set_option(asio::serial_port_base::character_size(Opt.m_CharacterSize));
        m_Port.set_option(asio::serial_port_base::parity(PARITIES[Opt.m_Parity]));
        m_Port.set_option(asio::serial_port_base::stop_bits(STOPBITS[Opt.m_StopBits]));
        m_Port.set_option(asio::serial_port_base::flow_control(asio::serial_port_base::flow_control::none));
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
	ERROR_TYPE LinuxSerial::Write(const uint8_t * pBuffer, size_t BufferSize)
	{
    	DUMP("WRITE", pBuffer, BufferSize);
		asio::write(m_Port, asio::buffer(pBuffer, BufferSize));
		return 0; //TODO
	}

	ERROR_TYPE LinuxSerial::Read(uint8_t * pBuffer, size_t MaxBytes, uint32_t TimeOutInMS /* = 0*/, size_t * pActualBytes /*= nullptr*/)
	{
    	ERROR_TYPE RetVal = SUCCESSFUL;
    	size_t     ActualBytes = 0;
		std::experimental::optional<asio::error_code> timer_result;
		asio::steady_timer timer(m_IO);
		timer.expires_from_now(std::chrono::milliseconds(TimeOutInMS));
		timer.async_wait([&timer_result](const asio::error_code& error) { timer_result.emplace(error); });

		std::experimental::optional<asio::error_code> read_result;
		asio::async_read(m_Port, asio::buffer(pBuffer, MaxBytes), 
        	[&read_result, &pActualBytes, &ActualBytes](const asio::error_code& error, size_t s) 
			{ 
				read_result.emplace(error); 
				if (pActualBytes)
				{
					*pActualBytes = s; 
				}
    			ActualBytes = s;
			});

		m_IO.reset();
		while (m_IO.run_one())
		{ 
    		if (read_result)
    		{
        		timer.cancel();
    		}
    		else if (timer_result)
    		{
        		m_Port.cancel();
        		RetVal = 1; //TODO
    		}
		}
    	if (ActualBytes)
    	{
        	DUMP("READ", pBuffer, ActualBytes, false);
    	}
    	return RetVal;
	}
	
	ERROR_TYPE LinuxSerial::Flush(FlushDirection Direction)
	{
		const int FLUSHES[] = { TCIFLUSH, TCOFLUSH, TCIOFLUSH };
		::tcflush(m_Port.lowest_layer().native_handle(), FLUSHES[Direction]);
    	
	}
	
	bool LinuxSerial::IsConnected()
	{
		return m_Port.is_open();
	}

	ERROR_TYPE LinuxSerial::Close()
	{
		m_Port.close();
	}

}