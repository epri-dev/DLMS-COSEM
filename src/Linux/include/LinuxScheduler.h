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
        
    protected:
        asio::io_service& m_IO;

    };
	
}
