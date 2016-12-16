#include <new>
#include <FreeRTOS.h>

#undef new

static uint32_t totalForNew = 0;

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