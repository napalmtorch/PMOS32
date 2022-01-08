#include <Kernel/HAL/Memory/Heap.h>
#include <Kernel/Core/Kernel.h>
using namespace System;
using namespace Graphics;

// size of full page table
static const uint32_t PAGE_SIZE = (4 * 1024 * 1024);

namespace HAL
{
    namespace Memory
    {
        /// @brief Initialize heap allocation manager @param size Total size in bytes @param max_entries Maximum amount of entries
        void HeapManager::Init(uint32_t size, uint32_t max_entries)
        {
            // limit and set heap size
            if (size > HEAP_MAX) { size = HEAP_MAX; }
            Size     = size;

            // set heap addresses as constants
            Physical = 0x01000000;
            Virtual  = 0xC1000000;

            // calculate page count
            uint32_t ps = 4 * 1024 * 1024;
            uint32_t pages = Size / ps;
            uint32_t v = Virtual, p = Physical;

            // map pages
            Debug::Info("Mapping pages for heap = PAGES: %d, SIZE: %d MB", pages, Core::Heap.Size / 1024 / 1024);
            for (uint32_t i = 0; i < pages; i++)
            {
                Core::VMM.KernelDirectory.Map(p, p, false);
                Core::VMM.KernelDirectory.Map(v, p, false);
                v += ps;
                p += ps;
            }

            // finished mapping pages - clear memory and print message
            memset((void*)Virtual, 0, Size);
            Debug::Info("Mapped (V=0x%8x, P=0x%8x) - (V=0x%8x, P=0x%8x)", Virtual, Physical, v - ps, p - ps);

            // create heap entry in pmm table
            Core::PMM.CreateEntry(&Core::PMM.UsedTable, Physical, Size, MemoryType::Heap);

            // setup table
            Entries   = (HeapEntry*)Virtual;
            TableSize = max_entries * sizeof(HeapEntry);
            Count     = 0;
            CountMax  = max_entries;

            // create mass free entry
            Entries[0] = { Core::PMM.Align(Virtual + TableSize), Size - TableSize, HeapType::Free };

            // finished
            Debug::OK("Initialized heap - %d MB, %d pages", Size / 1024 / 1024, pages);
        }

        // Print list of heap allocations
        void HeapManager::Print()
        {
            char memstr[32];
            memset(memstr, 0, 32);
            uint32_t used = UsedSize;
            if (used < 1024) { strcpy(memstr, "BYTES"); }
            else if (used >= 1024 && used < 1024 * 1024) { used /= 1024; strcpy(memstr, "KB"); }
            else if (used >= 1024 * 1024) { used = used / 1024 / 1024; strcpy(memstr, "MB"); }

            Debug::Write("------ ", Color4::Gray);
            Debug::Write("HEAP TABLE", Color4::Green);
            Debug::Write(" --------------------------------------------\n", Color4::Gray);
            Debug::Write("VIRTUAL         ", Color4::Yellow); Debug::PrintFormatted("0x%8x\n", Virtual);
            Debug::Write("PHYSICAL        ", Color4::Yellow); Debug::PrintFormatted("0x%8x\n", Physical);
            Debug::Write("SIZE            ", Color4::Yellow); Debug::PrintFormatted("%d MB\n", Size / 1024 / 1024);
            Debug::Write("USED            ", Color4::Yellow); Debug::PrintFormatted("%d %s\n", used, memstr);
            Debug::Write("COUNT           ", Color4::Yellow); Debug::PrintFormatted("%d/%d entries\n", Count, CountMax);
            Debug::Write("TABLE SIZE      ", Color4::Yellow); Debug::PrintFormatted("%d KB\n", TableSize / 1024);
            
            Debug::WriteLine("--------------------------------------------------------------", Color4::Gray);
            Debug::WriteChar('-');
            Debug::WriteLine(" ADDR       TYPE               SIZE", Color4::Magenta);

            for (uint32_t i = 0; i < CountMax; i++)
            {
                if (Entries[i].Address == 0) { continue; }
                if (Entries[i].Type != HeapType::Free) { continue; }

                char memstr[32];
                memset(memstr, 0, 32);

                uint32_t size = Entries[i].Size;
                if (size < 1024) { strcpy(memstr, "BYTES"); }
                else if (size >= 1024 && size < 1024 * 1024) { size /= 1024; strcpy(memstr, "KB"); }
                else if (size >= 1024 * 1024) { size = size / 1024 / 1024; strcpy(memstr, "MB"); }

                Debug::PrintFormatted("- 0x%8x %s        %d %s\n", Entries[i].Address, GetTypeString(Entries[i].Type), size, memstr);
            }

            for (uint32_t i = 0; i < CountMax; i++)
            {
                if (Entries[i].Address == 0) { continue; }
                if (Entries[i].Type == HeapType::Free) { continue; }

                char memstr[32];
                memset(memstr, 0, 32);

                uint32_t size = Entries[i].Size;
                if (size < 1024) { strcpy(memstr, "BYTES"); }
                else if (size >= 1024 && size < 1024 * 1024) { size /= 1024; strcpy(memstr, "KB"); }
                else if (size >= 1024 * 1024) { size = size / 1024 / 1024; strcpy(memstr, "MB"); }

                Debug::PrintFormatted("- 0x%8x %s        %d %s\n", Entries[i].Address, GetTypeString(Entries[i].Type), size, memstr);
            }

            Debug::WriteLine("--------------------------------------------------------------\n", Color4::Gray);
        }

        const char* HeapManager::GetTypeString(HeapType type)
        {
            switch (type)
            {
                case HeapType::Free:            { return "FREE       "; }
                case HeapType::Used:            { return "DEFAULT    "; }
                case HeapType::Array:           { return "ARRAY      "; }
                case HeapType::PointerArray:    { return "PTRARRAY   "; }
                case HeapType::Process:         { return "PROCESS    "; }
                case HeapType::Thread:          { return "THREAD     "; }
                case HeapType::ThreadStack:     { return "THREADSTACK"; }
                case HeapType::String:          { return "STRING     "; }
                case HeapType::Object:          { return "OBJECT     "; }
                default:                        { return "ERROR      "; }
            }
        }

        /// @brief Allocate region of memory @param size Size in bytes @param clear Clear allocated memory @param type Allocation type @return Pointer to allocated memory
        void* HeapManager::Allocate(uint32_t size, bool clear, HeapType type)
        {
            if (size == 0) { Debug::Error("Tried to allocate 0 bytes, type = 0x%2x", (uint32_t)type); return nullptr; }
            uint32_t aligned = Core::PMM.Align(size);
            HeapEntry* entry = GetAllocatedEntry(aligned);
            if (entry == nullptr) { Debug::Error("Out of memory"); return nullptr; }
            if (clear) { memset((void*)entry->Address, 0, aligned); }
            if (type != HeapType::Free) { entry->Type = type; }
            UsedSize += aligned;
            //Debug::Info("ALLOC - ADDR: 0x%8x, TYPE: 0x%2x, SIZE: %d bytes", entry->Address, (uint32_t)entry->Type, entry->Size);
            return (void*)entry->Address;
        }

        /// @brief Free region of memory @param ptr Pointer to allocated memory
        void HeapManager::Free(void* ptr)
        {
            if (ptr == nullptr) { return; }
            if (ptr == (void*)Entries[0].Address) { return; }
            for (uint32_t i = 0; i < CountMax; i++)
            {
                if (Entries[i].Address == 0 || Entries[i].Size == 0 || Entries[i].Type == HeapType::Free) { continue; }
                if (Entries[i].Address == (uint32_t)ptr)
                {
                    Entries[i].Type = HeapType::Free;
                    UsedSize -= Entries[i].Size;
                    Merge();
                    //Debug::Info("FREE -  ADDR: 0x%8x, SIZE: %d bytes", Entries[i].Address, Entries[i].Size);
                    return;
                }
            }

            Debug::Warning("Unable to free pointer 0x%8x", (uint32_t)ptr);
        }

        /// @brief Free allocated pointer array @param ptr Array pointer @param count Amount of array entries
        void HeapManager::FreeArray(void** ptr, uint32_t count)
        {
            if (ptr == nullptr) { return; }
            for (uint32_t i = 0; i < count; i++) 
            { 
                if (ptr[i] != nullptr) { Free(ptr[i]); }
            }
            Free(ptr);
        }

        /// @brief Get next available entry and mark as allocated @param size Size in bytes - 4K aligned
        HeapEntry* HeapManager::GetAllocatedEntry(uint32_t size)
        {
            for (uint32_t i = 1; i < CountMax; i++)
            {
                if (Entries[i].Address == 0 || Entries[i].Size == 0 || Entries[i].Type != HeapType::Free) { continue; }
                if (Entries[i].Size == size)
                {
                    Entries[i].Type = HeapType::Used;
                    return &Entries[i];
                }
            }

            HeapEntry* entry = CreateEntry(Entries[0].Address, size, HeapType::Used);
            Entries[0].Address += size;
            Entries[0].Size -= size;
            Entries[0].Type = HeapType::Free;
            return entry;

            /*
            for (uint32_t i = 0; i < CountMax; i++)
            {
                if (Entries[i].Address == 0 || Entries[i].Size == 0 || Entries[i].Type != HeapType::Free) { continue; }
                if (Entries[i].Size > size)
                {
                    CreateEntry(Entries[i].Address + size, Entries[i].Size - size, HeapType::Free);
                    Entries[i].Size = size;
                    Entries[i].Type = HeapType::Used;
                    return &Entries[i];
                }
            }
            return nullptr;
            */
        }

        void HeapManager::Merge()
        {
            for (uint32_t i = 0; i < CountMax; i++)
            {
                if (Entries[i].Address > 0 && Entries[i].Size > 0 && Entries[i].Type == HeapType::Free)
                {
                    HeapEntry* nearest = GetNearestEntry(&Entries[i]);

                    if (nearest != nullptr && nearest != &Entries[i])
                    {
                        if (Entries[i].Address > nearest->Address) { Entries[i].Address = nearest->Address; }
                        Entries[i].Size += nearest->Size;
                        DeleteEntry(nearest);
                    }
                }
            }
        }

        HeapEntry* HeapManager::GetNearestEntry(HeapEntry* entry)
        {
            if (entry == nullptr) { return nullptr; }
            if (entry->Address == 0 || entry->Size == 0 || entry->Type != HeapType::Free) { return nullptr; }
            
            for (uint32_t i = 0; i < CountMax; i++)
            {
                HeapEntry* temp = &Entries[i];

                if (temp != nullptr && temp != entry && temp->Address + temp->Size == entry->Address && temp->Type == HeapType::Free) { return temp; }
                if (temp != nullptr && temp != entry && entry->Address - entry->Size == temp->Address && temp->Type == HeapType::Free) { return temp; }
            }
            return nullptr;
        }

        /// @brief Create heap entry @param addr Address value @param size Size in bytes @param type Allocation type @return Pointer to entry
        HeapEntry* HeapManager::CreateEntry(uint32_t addr, uint32_t size, HeapType type)
        {
            int i = GetFreeIndex();
            if (i < 0 || i >= CountMax) { Debug::Error("Unable to get free index while creating heap entry"); return nullptr; }
            Entries[i].Address = addr;
            Entries[i].Size    = size;
            Entries[i].Type    = type;
            Count++;
            return &Entries[i];
        }

        /// @brief Delete heap entry @param Pointer to entry @return Success of deletion
        bool HeapManager::DeleteEntry(HeapEntry* entry)
        {
            for (uint32_t i = 0; i < CountMax; i++)
            {
                if (Entries[i].Address == 0 || Entries[i].Size == 0) { continue; }
                if (&Entries[i] == entry)
                {
                    Entries[i].Address = 0;
                    Entries[i].Size    = 0;
                    Entries[i].Type    = HeapType::Free;
                    Count--;
                    return true;
                }
            }

            Debug::Error("Unable to delete heap entry 0x%8x", (uint32_t)entry);
            return false;
        }

        /// @brief Get next null value index in entry array @return Index of entry
        int HeapManager::GetFreeIndex()
        {
            for (uint32_t i = 1; i < CountMax; i++)
            {
                if (Entries[i].Address == 0 && Entries[i].Size == 0 && Entries[i].Type == HeapType::Free)
                {
                    return i;
                }
            }
            return -1;
        }
    }
}