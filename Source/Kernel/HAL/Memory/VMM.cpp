#include <Kernel/HAL/Memory/VMM.h>
#include <Kernel/Core/Kernel.h>
using namespace System;
using namespace Graphics;

#define PAGE_COUNT 1024

EXTC
{
    extern uint32_t _boot_page_dir;
    extern uint32_t _kernel_page_dir;
    extern uint32_t _kernel_page_dir_end;

    extern void _write_cr3(uint32_t value);
    extern uint32_t _read_cr3();
}

namespace HAL
{
    namespace Memory
    {
        uint32_t DirectoryID = 0;

        void PageDirectory::Init(uint32_t pd_start)
        {
            Physical = pd_start;
            Entries = (uint32_t*)Physical;
            for (uint32_t i = 0; i < PAGE_COUNT; i++) { Write(Entries[i], 0 | 2); }
            ID = DirectoryID++;

            Debug::Info("Initialized page directory %d - START: 0x%8x", ID, Physical);
        }

        bool space_down = false;
        void PageDirectory::Print()
        {
            int pos = 0;

            Debug::Write("------ ", Color4::Gray);
            Debug::Write("VMM TABLE", Color4::Green);
            Debug::Write(" ---------------------------------------------\n", Color4::Gray);
            Debug::WriteLine(" PHYSICAL    VIRTUAL     ENTRIES", Color4::Magenta);


            for (uint32_t i = 0; i < PAGE_COUNT; i++)
            {
                if (!Tables[i].Used) { continue; }
                char temp[32];
                memset(temp, 0, 32);
                strhex(Tables[i].Physical, temp, true, 4);
                Debug::Write(" ");
                Debug::Write(temp, Color4::Yellow); Debug::Write("  ");
                strhex(Tables[i].Virtual, temp, true, 4);
                Debug::Write(temp, Color4::Green); Debug::Write("  ");
                strhex((uint32_t)Tables[i].Entries, temp, true, 4);
                Debug::Write(temp, Color4::Cyan); 
                Debug::WriteChar('\n');

                Core::Keyboard.TerminalOutput = false;
                pos++;
                if (pos >= 10)
                {              
                    while (true)
                    {
                        if (Core::Keyboard.IsKeyDown(Key::Space) && !space_down) { pos = 0; space_down = true; break; }
                        if (Core::Keyboard.IsKeyUp(Key::Space)) { space_down = false; }
                    }
                }
                Core::Keyboard.TerminalOutput = true;
            }

            Debug::WriteLine("--------------------------------------------------------------\n", Color4::Gray);
        }

        void PageDirectory::Map(uint32_t v_addr, uint32_t p_addr, bool msg)
        {
            uint32_t   start = p_addr;
            uint32_t   id    = v_addr >> 22;
            PageTable* table = GetFreePageTable();
            uint32_t   index = GetPageTableIndex(table); 

            if (table == nullptr) { Debug::Error("Unable to map page table"); return; }

            table->Entries  = (uint32_t*)((Physical + (PAGE_COUNT * 4)) + (index * (PAGE_COUNT * 4)));
            table->Physical = p_addr;
            table->Virtual  = v_addr;
            table->Used     = true;

            for (uint32_t i = 0; i < PAGE_COUNT; i++) { table->Entries[i] = p_addr | 3; p_addr += 0x1000; }
            Write(id, (uint32_t)table->Entries | 3);
            TableCount++;

            if (msg) { Debug::Info("Mapped(%d) physical 0x%8x to address 0x%8x(%d)", ID, start, v_addr, id); }
        }

        void PageDirectory::Unmap(void* v_ptr)
        {
            uint32_t id = (uint32_t)v_ptr >> 22;

            for (uint32_t i = 0; i < PAGE_COUNT; i++)
            {
                if (Tables[i].Used && Tables[i].Virtual == (uint32_t)v_ptr)
                {
                    Tables[i].Physical = 0;
                    Tables[i].Virtual  = 0;
                    Tables[i].Used     = false;

                    Write(id, 0 | 2);
                    Debug::Info("Un-mapped virtual address 0x%8x(ID = %d)", v_ptr, id);
                    return;
                }
            }

            Debug::Error("Unable to un-map page table at virtual address 0x%8x(ID = %d)", v_ptr, id);
        }

        void PageDirectory::Write(int index, uint32_t table)
        {
            Entries[index] = 0;
            Entries[index] = table;
        }

        uint32_t PageDirectory::Read(int index)
        {
            return Entries[index];
        }

        void PageDirectory::SetPhysical(uint32_t value)
        {
            Physical = value;
            Entries = (uint32_t*)Physical;
        }

        PageTable* PageDirectory::GetFreePageTable()
        {
            for (uint32_t i = 0; i < PAGE_COUNT; i++) { if (!Tables[i].Used) { return &Tables[i]; } }
            return nullptr;
        }

        int PageDirectory::GetPageTableIndex(PageTable* table)
        {
            for (uint32_t i = 0; i < PAGE_COUNT; i++) { if (&Tables[i] == table) { return i; } }
            return -1;
        }

        uint32_t PageDirectory::GetPhysical() { return Physical; }

        PageTable* PageDirectory::GetPageTables() { return Tables; }

        // -----------------------------------------------------------------

        void VirtualMemoryManager::Init()
        {
            uint32_t phys = ((uint32_t)(&_kernel_page_dir) - KBASE_VIRTUAL);
            KernelDirectory.Init(phys);
            KernelDirectory.Map(0, 0);
            KernelDirectory.Map(0x00400000, 0x00400000);
            KernelDirectory.Map(0x00800000, 0x00800000);
            KernelDirectory.Map(0x00C00000, 0x00C00000);
            KernelDirectory.Map(KBASE_VIRTUAL, 0);
            
            _write_cr3(KernelDirectory.Physical);

            KernelDirectory.SetPhysical(0x00800000);
            memcpy((void*)KernelDirectory.Physical, (void*)phys, 0x1000);
            _write_cr3(KernelDirectory.Physical);

            memset((void*)phys, 0, 0x1000);
            memset(&_boot_page_dir, 0, PAGE_COUNT * sizeof(uint32_t));
            Debug::OK("Initialized VMM");
        }
    }
}