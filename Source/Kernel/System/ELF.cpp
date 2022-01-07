#include <Kernel/System/ELF.h>
#include <Kernel/Core/Kernel.h>
using namespace HAL;
using namespace System;
using namespace Graphics;

namespace System
{
    namespace ELF
    {   
        const char ValidID[4] = { 0x7F, 'E', 'L', 'F' };

        bool Validate(ELFHeader* header) { return !memcmp(header->Identifier.ID, ValidID, 4); }

        bool Init(uint8_t* data, uint32_t size, Process* proc)
        {
            Debug::Info("Reading ELF file...");

            if (!Validate((ELFHeader*)data)) { Debug::Error("File is not ELF format"); return false; }

            ELFHeader* header = (ELFHeader*)data;
            ELFProgramHeader* prog_header = (ELFProgramHeader*)(data + header->PHTable);

            if (header->Type != 2) { Debug::Error("ELF file is not executable, type = 0x%2x", header->Type); return false; }

            Debug::Info("PROG DATA SIZE: %d", size + 0x1000);
            uint8_t* prog_data = (uint8_t*)(uint8_t*)Core::Heap.Allocate(size + 0x1000, true, Memory::HeapType::Array);
            proc->ProgramData = prog_data;
            proc->ProgramSize = size + 0x1000;
            proc->Virtual     = header->Entry;

            for (uint32_t i = 0; i < header->PHEntryCount; i++, prog_header++)
            {
                switch (prog_header->Type)
                {
                    case 0: { break; }
                    case 1:
                    {
                        Core::VMM.KernelDirectory.Map(prog_header->Virtual, (uint32_t)prog_data - KBASE_VIRTUAL);
                        memcpy((void*)prog_header->Virtual, (void*)((uint32_t)data + prog_header->Offset), size + 0x1000);

                        Debug::Info("ELF Load: OFFSET = 0x%8x, VIRT = 0x%8x, PHYS = 0x%8x, SIZE = %d", 
                                        prog_header->Offset, prog_header->Virtual, prog_header->Physical, prog_header->SegmentSize);

                        prog_data += Core::PMM.Align(prog_header->SegmentSize);
                        break;
                    }
                    default: { Debug::Error("Unsupported ELF program header type 0x%8x", prog_header->Type); return false; }
                }
            }

            proc->Threads[0]->Protocol = (Threading::ThreadProtocol)header->Entry;
            return true;
        }
    }
}