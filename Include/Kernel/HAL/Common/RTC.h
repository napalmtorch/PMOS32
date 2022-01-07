#pragma once
#include <Lib/Types.h>
#include <Lib/Time.h>
#include <Kernel/HAL/Interrupts/IDT.h>

namespace HAL
{
    namespace RTC
    {
        void Init();
        void Fetch();
        void Callback(Registers32* regs);
        DateTime GetCurrentTime();
    }
}