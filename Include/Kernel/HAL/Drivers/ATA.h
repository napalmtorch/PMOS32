#pragma once
#include <Lib/Types.h>
#include <Kernel/HAL/Interrupts/IDT.h>

namespace HAL
{
    namespace Drivers
    {
        class ATAController
        {
            public:
                uint32_t SectorSize;
                bool     Initialized = false;

            public: 
                void Init();
                static void Callback(Registers32* regs);

            public:
                bool Identify();
                bool Read(uint64_t sector, uint32_t count, uint8_t* buffer);
                bool Write(uint64_t sector, uint32_t count, uint8_t* buffer);
                bool Fill(uint64_t sector, uint32_t count, uint8_t* buffer);
        };
    }
}