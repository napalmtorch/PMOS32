#pragma once
#include <Lib/Types.h>

#define MULTIBOOT_MEMORY_AVAILABLE              1
#define MULTIBOOT_MEMORY_RESERVED               2
#define MULTIBOOT_MEMORY_ACPI_RECLAIMABLE       3
#define MULTIBOOT_MEMORY_NVS                    4
#define MULTIBOOT_MEMORY_BADRAM                 5

namespace HAL
{
    namespace Memory
    {
        enum class MemoryType : uint8_t
        {
            Free,
            Allocated,
            Reserved,
            Kernel,
            Framebuffer,
            ACPI,
            ACPIFree,
            Heap,
            Error,
        };

        enum class MemoryMapType : uint32_t
        {
            Unspecified,
            Available,
            Reserved,
            ACPIFree,
            NVS,
            Error,
        };

        typedef struct
        {
            uint32_t      Size;
            uint32_t      Address;
            uint32_t      AddressHigh;
            uint32_t      Length;
            uint32_t      LengthHigh;
            uint32_t      Type;
        } PACKED MemoryMapEntry;

        typedef struct
        {
            uint32_t   Address;
            uint32_t   Size;
            MemoryType Type;
        } PACKED PhysicalMemoryEntry;

        typedef struct
        {
            uint32_t Address;
            uint32_t Size;
            uint32_t Count;
            uint32_t CountMax;
            PhysicalMemoryEntry* Entries;
        } PACKED PhysicalMemoryTable;

        class PhysicalMemoryManager
        {
            public:
                PhysicalMemoryTable FreeTable;
                PhysicalMemoryTable UsedTable;

            private:
                uint32_t UsableAddress;
                uint32_t UsableSize;
                uint32_t DataAddress;
                uint32_t DataSize;
                bool     Initialized;

            public:
                void Init();
                void ReadMemoryMap();

            public:
                PhysicalMemoryEntry* CreateEntry(PhysicalMemoryTable* table, uint32_t addr, uint32_t size, MemoryType type);
                bool DeleteEntry(PhysicalMemoryTable* table, PhysicalMemoryEntry* entry);
                PhysicalMemoryEntry GetFirstEntryByType(MemoryType type);
                bool IsAddressValid(uint32_t addr);

            public:
                static uint32_t Align(uint32_t value);
                static const char* GetTypeString(MemoryType type);
                void PrintTable(PhysicalMemoryTable* table);

            private:
                bool IsEntryAvailable(PhysicalMemoryTable* table, int index);
                int GetFreeIndex(PhysicalMemoryTable* table);
        };
    }
}