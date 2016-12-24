#include <new>
#include <FreeRTOS.h>
#include <../CMSIS_RTOS/cmsis_os.h>

#include "FreeRTOSConfig.h"

#undef new

static uint32_t totalForNew = 0;
static uint32_t totalMallocFailures = 0;
static uint32_t totalStackOverflowFailures = 0;

#if( configUSE_MALLOC_FAILED_HOOK == 1 )
extern "C" void vApplicationMallocFailedHook(void)
{
    totalMallocFailures++;
}
#endif

#if( configCHECK_FOR_STACK_OVERFLOW  == 1 )
extern "C" void vApplicationStackOverflowHook(TaskHandle_t xTask,
                                              signed char *pcTaskName)
{
    totalStackOverflowFailures++;
}
#endif


void * operator new(std::size_t size) throw (std::bad_alloc) {
    void *p = pvPortMalloc(size);
    totalForNew += size;
    return p;
}

void * operator new(std::size_t size, const std::nothrow_t& nothrow_constant) throw() {
    return pvPortMalloc(size);
}

void * operator new[](std::size_t size) throw (std::bad_alloc) {
    void *p = pvPortMalloc(size);
    return p;
}

void * operator new[](std::size_t size, const std::nothrow_t& nothrow_constant) throw() {
    return pvPortMalloc(size);
}

void operator delete(void* ptr) throw () {
    vPortFree(ptr);
}

void operator delete(void* ptr, const std::nothrow_t& nothrow_constant) throw() {
    vPortFree(ptr);
}

void operator delete[](void* ptr) throw () {
    vPortFree(ptr);
}

void operator delete[](void* ptr, const std::nothrow_t& nothrow_constant) throw() {
    vPortFree(ptr);
}