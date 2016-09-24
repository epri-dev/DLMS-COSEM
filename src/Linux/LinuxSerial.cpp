#include "LinuxSerial.h"

namespace EPRI
{
	LinuxSerial::LinuxSerial(IBaseLibrary * pLibrary) : 
		BaseComponent(pLibrary),
		m_pPort(NULL)
	{
	}
	
	LinuxSerial::~LinuxSerial()
	{
	}

	ERROR_TYPE LinuxSerial::Open(ISerial::SerialPort Port, ISerial::BaudRate Baud)
	{
		m_pPort = new asio::serial_port(m_IO, "/tmp/ttyS10");
	}
	
	ERROR_TYPE LinuxSerial::Write(const uint8_t * pBuffer, size_t BufferSize)
	{
		asio::write(*m_pPort, asio::buffer(pBuffer, BufferSize));
	}
	
	ERROR_TYPE LinuxSerial::Close()
	{
		delete m_pPort;
	}

}