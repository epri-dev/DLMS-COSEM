#include "LinuxCore.h"
#include "LinuxSimpleTimer.h"

namespace EPRI
{
    LinuxCore::LinuxCore(asio::io_service& IO) :
        m_IP(IO), m_Serial(IO)
	{
	}
	
	LinuxCore::~LinuxCore()
	{
	}

	ISerial * LinuxCore::GetSerial()
	{
		return &m_Serial;
	}

    IIP * LinuxCore::GetIP()
    {
        return &m_IP;
    }
	
	std::shared_ptr<ISimpleTimer> LinuxCore::CreateSimpleTimer(bool bUseHeap /* = true*/)
	{
		// TODO - Embedded memory management
		
		return std::shared_ptr<ISimpleTimer>(new LinuxSimpleTimer);
	}

}