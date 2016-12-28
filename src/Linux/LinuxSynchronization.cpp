
#include "LinuxSynchronization.h"

namespace EPRI
{
    //
    // LinuxSemaphore
    //
    LinuxSemaphore::LinuxSemaphore()
    {
        ::sem_init(&m_Semaphore, 0, 1);
    }
    
    LinuxSemaphore::~LinuxSemaphore()
    {
        ::sem_destroy(&m_Semaphore);
    }

    bool LinuxSemaphore::Take()
    {
        return 0 == ::sem_wait(&m_Semaphore);
    }
    
    bool LinuxSemaphore::Give()
    {
        return 0 == ::sem_post(&m_Semaphore);
    }

    LinuxSynchronization::LinuxSynchronization()
    {
    }
    
    LinuxSynchronization::~LinuxSynchronization()
    {
    }
        
    ISemaphorePtr LinuxSynchronization::CreateSemaphore()
    {
        return ISemaphorePtr(new LinuxSemaphore);
    }
	
}
