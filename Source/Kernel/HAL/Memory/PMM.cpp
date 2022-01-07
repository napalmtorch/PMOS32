#include <Kernel/HAL/Memory/PMM.h>
#include <Kernel/Core/Kernel.h>
using namespace System;
using namespace Graphics;

#define PMM_ALIGN          0x1000
#define PMM_SIZE_MIN       64 * 1024 * 1024
#define PMM_COUNT_MAX_FREE 8192
#define PMM_COUNT_MAX_USED 8192

namespace HAL
{
    namespace Memory
    {
        // Initialize physical memory manager
        void PhysicalMemoryManager::Init()
        {
            ReadMemoryMap();

            Debug::OK("Initialized PMM - %d MB free", DataSize / 1024 / 1024);
        }

        void PhysicalMemoryManager::ReadMemoryMap()
        {
            // locate available position for table
            bool located = false;

            UsableAddress = Align(0x00C00000);

            MemoryMapEntry* memmap = (MemoryMapEntry*)Kernel::Multiboot.mmap_addr;
            for (uint32_t i = 0; i < Kernel::Multiboot.mmap_len / sizeof(MemoryMapEntry); i++)
            {  
                // located usable entry
                Debug::Info("MMAP ENTRY: 0x%8x, 0x%2x, %d bytes", memmap[i].Address, memmap[i].Type, memmap[i].Length);
                if (memmap[i].Type == (uint32_t)MemoryMapType::Available && memmap[i].Length >= PMM_SIZE_MIN && !located)
                {
                    UsableSize    = memmap[i].Length - Kernel::GetSize();
                    located       = true;
                }
            }

            // check if able to locate enough memory to start
            if (!located) { Debug::Error("Not enough memory to start PMM"); return; }
            Debug::Info("Validated memory - ADDR: 0x%8x, SIZE: %d bytes", UsableAddress, UsableSize);

            // create used entry table based on free memory located above
            UsedTable.Address  = UsableAddress;
            UsedTable.CountMax = PMM_COUNT_MAX_USED;
            UsedTable.Count    = 0;
            UsedTable.Size     = UsedTable.CountMax * sizeof(PhysicalMemoryEntry); 
            UsedTable.Entries  = (PhysicalMemoryEntry*)UsedTable.Address;
            memset((void*)UsedTable.Address, 0, UsedTable.Size);
            Debug::Info("Created PMM used entry table");

            // create free entry table
            FreeTable.Address  = UsedTable.Address + UsedTable.Size;
            FreeTable.CountMax = PMM_COUNT_MAX_FREE;
            FreeTable.Count    = 0;
            FreeTable.Size     = FreeTable.CountMax * sizeof(PhysicalMemoryEntry);
            FreeTable.Entries  = (PhysicalMemoryEntry*)FreeTable.Address;
            memset((void*)FreeTable.Address, 0, FreeTable.Size);
            Debug::Info("Created PMM free entry table");

            // set data boundaries
            DataAddress = FreeTable.Address + FreeTable.Size;
            DataSize    = UsableSize - (UsedTable.Size + FreeTable.Size);
            
            // map kernel
            CreateEntry(&UsedTable, Kernel::GetStartAddress(), Kernel::GetEndAddress(), MemoryType::Kernel);

            // map tables
            CreateEntry(&UsedTable, UsedTable.Address, UsedTable.Size, MemoryType::Reserved);
            CreateEntry(&UsedTable, FreeTable.Address, FreeTable.Size, MemoryType::Reserved);

            // map secondary entries
            for (uint32_t i = 0; i < Kernel::Multiboot.mmap_len / sizeof(MemoryMapEntry); i++)
            {
                MemoryMapEntry* entry = &memmap[i];
                MemoryType type = MemoryType::Free;
                if (entry->Address == UsableAddress)
                {
                    (void)CreateEntry(&FreeTable, DataSize, DataSize, MemoryType::Free);
                    continue; 
                }

                switch (entry->Type)
                {
                    case 1: { continue; }
                    case 2: { type = MemoryType::Reserved; break; }
                    case 3: { type = MemoryType::ACPIFree; break; }
                    case 4: { type = MemoryType::ACPI; break; }
                    default: { Debug::Error("Unknown memory map type 0x%8x", entry->Type); return; }
                }

                PhysicalMemoryEntry* pmm_entry = nullptr;
                if (entry->Address >= Kernel::GetEndAddress()) { pmm_entry = CreateEntry(&UsedTable, entry->Address, entry->Length, type); }
            }

            Debug::OK("Finished reading memory map");
        }

        PhysicalMemoryEntry* PhysicalMemoryManager::CreateEntry(PhysicalMemoryTable* table, uint32_t addr, uint32_t size, MemoryType type)
        {
            int index = GetFreeIndex(table);
            if (index < 0 || index >= table->CountMax) { Debug::Error("Index out of bounds while creating PMM entry"); return nullptr; }

            table->Entries[index].Address = addr;
            table->Entries[index].Size    = size;
            table->Entries[index].Type    = type;
            table->Count++;
            return &table->Entries[index];
        }

        bool PhysicalMemoryManager::DeleteEntry(PhysicalMemoryTable* table, PhysicalMemoryEntry* entry)
        {
            if (table == nullptr) { return false; }
            if (entry == nullptr) { return false; }
            for (uint32_t i = 0; i < table->CountMax; i++)
            {
                if (&table->Entries[i] == entry)
                {
                    table->Entries[i].Address = 0;
                    table->Entries[i].Size    = 0;
                    table->Entries[i].Type    = MemoryType::Free;
                    return true;
                }
            }

            return false;
        }

        PhysicalMemoryEntry PhysicalMemoryManager::GetFirstEntryByType(MemoryType type)
        {
            for (uint32_t i = 0; i < UsedTable.CountMax; i++)
            {
                if (IsEntryAvailable(&UsedTable, i)) { continue; }
                if (UsedTable.Entries[i].Type == type)
                {
                    PhysicalMemoryEntry output = UsedTable.Entries[i];
                    return output;
                }
            }

            return { 0, 0, MemoryType::Free };
        }

        bool PhysicalMemoryManager::IsAddressValid(uint32_t addr)
        {
            if (addr >= UsableAddress && addr < UsableAddress + UsableSize) { return true; }
            return false;
        }

        uint32_t PhysicalMemoryManager::Align(uint32_t value)
        {
            uint32_t out = value;
            out &= 0xFFFFF000;
            if (out < value) { out += PMM_ALIGN; }
            return out;
        }

        bool PhysicalMemoryManager::IsEntryAvailable(PhysicalMemoryTable* table, int index)
        {
            if (index < 0 || index >= table->CountMax) { return false; }
            if (table->Entries[index].Address != 0) { return false; }
            if (table->Entries[index].Size != 0) { return false; }
            if (table->Entries[index].Type != MemoryType::Free) { return false; }
            return true;
        }

        int PhysicalMemoryManager::GetFreeIndex(PhysicalMemoryTable* table)
        {
            for (uint32_t i = 0; i < table->CountMax; i++)
            {
                if (IsEntryAvailable(table, i)) { return i; }
            }
            return -1;
        }

        const char* PhysicalMemoryManager::GetTypeString(MemoryType type)
        {
            switch (type)
            {
                case MemoryType::Free:        { return "FREE       "; }
                case MemoryType::Reserved:    { return "RESERVED   "; }
                case MemoryType::Allocated:   { return "ALLOCATED  "; }
                case MemoryType::Kernel:      { return "KERNEL     "; }
                case MemoryType::Framebuffer: { return "FRAMEBUFFER"; }
                case MemoryType::ACPI:        { return "ACPI_NVS   "; }
                case MemoryType::ACPIFree:    { return "ACPI_FREE  "; }
                case MemoryType::Heap:        { return "HEAP       "; }
                default:                      { return "ERROR      "; }
            }
        }

        void PhysicalMemoryManager::PrintTable(PhysicalMemoryTable* table)
        {
            Debug::Write("------ ", Color4::Gray);
            Debug::Write("PMM TABLE", Color4::Green);
            Debug::Write(" ---------------------------------------------\n", Color4::Gray);
            Debug::Write("ADDR            ", Color4::Yellow); Debug::PrintFormatted("0x%8x\n", table->Address);
            Debug::Write("COUNT           ", Color4::Yellow); Debug::PrintFormatted("%d/%d entries\n", table->Count, table->CountMax);
            Debug::Write("SIZE            ", Color4::Yellow); Debug::PrintFormatted("%d KB\n", table->Size / 1024);
            Debug::WriteLine("--------------------------------------------------------------", Color4::Gray);
            Debug::WriteChar('-');
            Debug::WriteLine(" ADDR       TYPE        SIZE", Color4::Magenta);


            for (uint32_t i = 0; i < table->CountMax; i++)
            {
                if (IsEntryAvailable(table, i)) { continue; }

                char memstr[32];
                memset(memstr, 0, 32);

                uint32_t size = table->Entries[i].Size;
                if (size < 1024) { strcpy(memstr, "BYTES"); }
                else if (size >= 1024 && size < 1024 * 1024) { size /= 1024; strcpy(memstr, "KB"); }
                else if (size >= 1024 * 1024 && size < 1024 * 1024 * 1024) { size = size / 1024 / 1024; strcpy(memstr, "MB"); }

                Debug::PrintFormatted("- 0x%8x %s %d %s\n", table->Entries[i].Address, GetTypeString(table->Entries[i].Type), size, memstr);
            }
            Debug::WriteLine("--------------------------------------------------------------\n", Color4::Gray);
        }
    }
}