#pragma once
#include <Lib/Types.h>
#include <Kernel/HAL/Interrupts/IDT.h>

namespace HAL
{
    namespace Drivers
    {
        class PS2Mouse
        {
            private:
                uint32_t X, Y;
                uint8_t  Offset, Cycle, Buttons;
                int8_t   Buffer[3];
                bool     LeftBtn, RightBtn;

            public:
                void Init();
                void Disable();

            private:
                static void Callback(Registers32* regs);
                void        OnInterrupt();
                void        OnMove(int8_t x, int8_t y);
                void        Wait(uint8_t a_type);
                void        Write(uint8_t a_write);
                uint8_t     Read();

            public:
                void SetPosition(uint32_t x, uint32_t y);

            public:
                bool IsLeftPressed();
                bool IsRightPressed();
                uint32_t GetX();
                uint32_t GetY();
        };
    }
}