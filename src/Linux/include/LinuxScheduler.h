#pragma once

#include <asio.hpp>

#include "IScheduler.h"

namespace EPRI
{
    class LinuxScheduler : public IScheduler
    {
    public:
        LinuxScheduler() = delete;
        LinuxScheduler(asio::io_service& IO);
        virtual ~LinuxScheduler();
        
        virtual void Post(PostFunction Handler);
        virtual void Sleep(uint32_t MSToSleep);
       
    protected:
        asio::io_service& m_IO;

    };
	
}
