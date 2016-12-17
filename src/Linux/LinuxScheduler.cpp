#include <unistd.h>

#include "LinuxScheduler.h"

namespace EPRI
{
    LinuxScheduler::LinuxScheduler(asio::io_service& IO) :
        m_IO(IO)
    {
    }
    
    LinuxScheduler::~LinuxScheduler()
    {
    }
        
    void LinuxScheduler::Post(PostFunction Handler)
    {
        m_IO.post(Handler);
    }
    
    void LinuxScheduler::Sleep(uint32_t MSToSleep)
    {
        ::usleep(MSToSleep * 1000L);
    }

}