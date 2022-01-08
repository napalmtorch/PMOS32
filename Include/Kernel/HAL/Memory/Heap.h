#pragma once
#include <Lib/Types.h>
#include <Kernel/HAL/Memory/PMM.h>

#define HEAP_MAX 1024 * 1024 * 1024

namespace HAL
{
    namespace Memory
    {
        enum class HeapType : uint8_t
        {
            Free,
            Used,
            Array,
            PointerArray,
            Process,
            Thread,
            ThreadStack,   
            String,
            Object,
            Error,
        };
        

        typedef struct
        {
            uint32_t Address;
            uint32_t Size;
            HeapType Type;
        } PACKED HeapEntry;

        class HeapManager
        {
            public:
                uint32_t   Size;
                uint32_t   Physical;
                uint32_t   Virtual;
                uint32_t   TableSize;
                uint32_t   Count;
                uint32_t   CountMax;
                uint32_t   UsedSize;
                HeapEntry* Entries;

            public:
                void Init(uint32_t size, uint32_t max_entries);
                void Print();
                static const char* GetTypeString(HeapType type);

            public:
                void* Allocate(uint32_t size, bool clear = false, HeapType type = HeapType::Used);
                void  Free(void* ptr);
                void  FreeArray(void** ptr, uint32_t count);
                void  Merge();

            private:
                HeapEntry* GetAllocatedEntry(uint32_t size);
                HeapEntry* GetNearestEntry(HeapEntry* entry);
                HeapEntry* CreateEntry(uint32_t addr, uint32_t size, HeapType type);
                bool       DeleteEntry(HeapEntry* entry);
                int        GetFreeIndex();
        };
    }
}