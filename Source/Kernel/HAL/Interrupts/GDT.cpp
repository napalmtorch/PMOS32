#include <Kernel/HAL/Interrupts/GDT.h>
#include <Kernel/Core/Kernel.h>
using namespace System;

EXTC
{
    uint32_t _gdt_table;

    extern void _gdt_flush();
    extern void _reload_segments();
}

namespace HAL
{
    namespace GDT
    {
        // Amount of entries
        const uint32_t Count = 3;

        // Entry array
        GDTEntry*      Entries;

        // Register pointer
        GDTRegister*   GDTR;

        // Initialize global descriptor tables
        void Init()
        {
            // set pointers
            Entries = (GDTEntry*)0x810;
            GDTR    = (GDTRegister*)0x800;

            _gdt_table = (uint32_t)GDTR;
            Debug::Info("ENTRIES: 0x%8x GDTR: 0x%8x", Entries, (uint32_t)GDTR);

            memset(&Entries, 0, sizeof(GDTEntry) * Count);

            GDTR->Base  = (uint32_t)Entries;
            GDTR->Limit = (sizeof(GDTEntry) * Count) - 1;

            SetDescriptor(0, 0, 0, 0, 0);
            SetDescriptor(1, 0, 0xFFFFFFFF, 0x9A, 0xCF);
            SetDescriptor(2, 0, 0xFFFFFFFF, 0x92, 0xCF);

            _gdt_flush();
            _reload_segments();
            Debug::OK("Initialized GDT");
        }

        /// @brief Set GDT descriptor @param num Entry index @param base Base address @param limit End address @param access Access modifiers @param gran Granularity modifiers
        void SetDescriptor(uint32_t num, uint32_t base, uint32_t limit, uint8_t access, uint8_t gran)
        {
            Entries[num].BaseLow    = (base & 0xFFFF);
            Entries[num].BaseMiddle = (base >> 16) & 0xFF;
            Entries[num].BaseHigh   = (base >> 24) & 0xFF;

            Entries[num].LimitLow    = (limit & 0xFFFF);
            Entries[num].Granularity = ((limit >> 16) & 0x0F);
            Entries[num].Granularity |= (gran & 0xF0);
            Entries[num].Access      = access;
            Debug::Info("GDT 0x%2x: BASE: 0x%8x, LIMIT: 0x%8x, FLAGS: 0x%2x%2x", num, base, limit, access, gran);
        }
    }
}