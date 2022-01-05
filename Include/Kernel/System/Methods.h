#pragma once
#include <Lib/Types.h>
#include <Kernel/HAL/Memory/Heap.h>

namespace System
{
    typedef struct
    {
        const char* Name;
        uint32_t    Address;
    } PACKED MethodEntry;

    namespace ExternalMethods
    {
        void* MEM_ALLOCATE(uint32_t size, bool clear, HAL::Memory::HeapType type);
        void  MEM_FREE(void* ptr);
        void  VPRINTF(const char* str, va_list args);
    }

    namespace MethodLibrary
    {
        extern MethodEntry* Entries;
        extern uint32_t Count;

        void Init();
        void Register(const char* name, uint32_t addr);
    }
}