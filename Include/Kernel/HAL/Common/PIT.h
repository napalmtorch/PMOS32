#pragma once
#include <Lib/Types.h>
#include <Kernel/HAL/Interrupts/IDT.h>

namespace HAL
{
    namespace PIT
    {
        void Init(uint32_t freq, InterruptCallback callback);
        void Disable();
        void Calculate();

        uint64_t GetTicks();
        uint32_t GetFrequency();
    }
}