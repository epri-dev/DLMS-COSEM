#pragma once

#include <semaphore.h>

#include "ISynchronization.h"

namespace EPRI
{
    class LinuxSemaphore : public ISemaphore
    {
    public:
        LinuxSemaphore();
        virtual ~LinuxSemaphore();
        //
        // ISemaphore
        //
        virtual bool Take();
        virtual bool Give();
        
    protected:
        sem_t   m_Semaphore;
    };

    class LinuxSynchronization : public ISynchronization
    {
    public:
        LinuxSynchronization();
        virtual ~LinuxSynchronization();
        
        virtual ISemaphorePtr CreateSemaphore();

    };

}
