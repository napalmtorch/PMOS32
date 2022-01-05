#pragma once
#include <Lib/Types.h>

namespace HAL
{
    namespace Memory
    {
        typedef struct
        {
            uint32_t  Physical;
            uint32_t  Virtual;
            uint32_t* Entries;
            bool      Used;
        } PACKED PageTable;

        class PageDirectory
        {
            friend class VirtualMemoryManager;

            private:
                uint32_t  ID;
                uint32_t  Physical;
                uint32_t  TableCount;
                uint32_t* Entries;
                PageTable Tables[1024];

            public:
                void     Init(uint32_t pd_start);
                void     Print();
                void     Map(uint32_t v_addr, uint32_t p_addr, bool msg = true);
                void     Unmap(void* v_ptr);
                void     Write(int index, uint32_t table);
                uint32_t Read(int index);
                void     SetPhysical(uint32_t value);
                PageTable*    GetFreePageTable();
                int           GetPageTableIndex(PageTable* table);
                uint32_t      GetPhysical();
                PageTable*    GetPageTables();
        };

        class VirtualMemoryManager
        {
            friend class PageDirectory;

            public:
                PageDirectory KernelDirectory;

            public:
                void Init();
        };
    }
}