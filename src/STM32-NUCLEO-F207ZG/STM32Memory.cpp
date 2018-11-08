#include <memory>
#include "STM32Memory.h"

namespace EPRI
{
    STM32Memory::STM32Memory()
    {
    }
	
    STM32Memory::~STM32Memory()
    {
    }

    void * STM32Memory::Alloc(size_t Size)
    {
        return std::calloc(Size, 1);
    }

    ERROR_TYPE STM32Memory::Free(void* p)
    {
        std::free(p);
        p = nullptr;
        return SRC_DONT_CARE;
    }
}
