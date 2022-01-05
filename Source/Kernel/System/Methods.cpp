#include <Kernel/System/Methods.h>
#include <Kernel/Core/Kernel.h>
using namespace HAL;
using namespace Graphics;

namespace System
{
    namespace MethodLibrary
    {
        MethodEntry* Entries;
        uint32_t Count;

        int GetFreeIndex();

        void Init()
        {
            Count   = 0;
            Entries = (MethodEntry*)Core::Heap.Allocate(sizeof(MethodEntry) * 4096, true, Memory::HeapType::Array);

            Register("MEM_ALLOCATE", (uint32_t)ExternalMethods::MEM_ALLOCATE);
            Register("MEM_FREE", (uint32_t)ExternalMethods::MEM_FREE);
            Register("VPRINTF", (uint32_t)ExternalMethods::VPRINTF);

            Debug::OK("Initialized method library");
        }

        void Register(const char* name, uint32_t addr)
        {
            int i = GetFreeIndex();
            if (i < 0 || i >= 4096) { Debug::Error("Maximum amount of registered methods reached"); return; }
            Entries[i].Name    = name;
            Entries[i].Address = addr;
            Count++;
            Debug::Info("Registered method - ADDR: 0x%8x, NAME: '%s'", addr, name);
        }

        int GetFreeIndex()
        {
            for (uint32_t i = 0; i < 4096; i++)
            {
                if (Entries[i].Address == 0 && Entries[i].Name == nullptr) { return i; }
            }
            return -1;
        }
    }

    namespace ExternalMethods
    {
        void* MEM_ALLOCATE(uint32_t size, bool clear, HAL::Memory::HeapType type) { return Core::Heap.Allocate(size, clear, type); }

        void MEM_FREE(void* ptr) { Core::Heap.Free(ptr); }

        void VPRINTF(const char* str, va_list args) { Core::Terminal.PrintFormattedV(str, args); }
    }

}