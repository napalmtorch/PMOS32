#include <Kernel/HAL/Drivers/PS2Mouse.h>
#include <Kernel/Core/Kernel.h>
using namespace System;

namespace HAL
{
    namespace Drivers
    {
        void PS2Mouse::Init()
        {
            memset(Buffer, 0, 3);
            Offset   = 0;
            Cycle    = 0;
            Buttons  = 0;
            LeftBtn  = false;
            RightBtn = false;

            uint8_t status;
            Wait(1);
            Ports::Write8(0x64, 0xA8);

            Wait(1);
            Ports::Write8(0x64, 0x20);
            Wait(0);
            status = (Ports::Read8(0x60) | 2);
            
            Wait(1);
            Ports::Write8(0x64, 0x60);
            Wait(1);
            Ports::Write8(0x60, status);

            Write(0xF6);
            (void)Read();

            Write(0xF4);
            (void)Read();

            // register interrupt
            IDT::RegisterInterrupt(IRQ12, Callback);
        }

        void PS2Mouse::Disable()
        {
            IDT::UnregisterInterrupt(IRQ12);
        }

        void PS2Mouse::Callback(Registers32* regs)
        {
            Core::Mouse.OnInterrupt();
        }

        void PS2Mouse::OnInterrupt()
        {
            uint8_t status = Ports::Read8(0x64);
            Wait(1);
            if ((!(status & 1)) == 1) { Cycle = 0; return; }
            if ((!(status & 2)) == 0) { Cycle = 0; return; }
            if (!(status & 0x20))     { Cycle = 0; return; }

            if (Cycle == 0) { Buffer[0] = Read(); }
            else if (Cycle == 1) { Buffer[1] = Read(); }
            else if (Cycle == 2)
            {
                Buffer[2] = Read();
                OnMove(Buffer[1], -Buffer[2]);
                LeftBtn  = ((Buffer[0] & 0b00000001));
                RightBtn = ((Buffer[0] & 0b00000010) >> 1);
                Cycle = 0;
                return;
            }
            else { Debug::Error("PS/2 mouse cycle overflow exception"); return; }

            Cycle++;
        }

        void PS2Mouse::OnMove(int8_t x, int8_t y)
        {
            X += x;
            Y += y;
        }

        void PS2Mouse::Wait(uint8_t a_type)
        {
            uint32_t time_out = 100000;
            if (a_type == 0)
            {
                while (time_out--) { if ((Ports::Read8(0x64) & 1) == 1) { return; } }
                return;
            }
            else
            {
                while (time_out--) { if ((Ports::Read8(0x64) & 2) == 0) { return; } }
                return;
            }
        }

        void PS2Mouse::Write(uint8_t a_write)
        {
            Wait(1);
            Ports::Write8(0x64, 0xD4);
            Wait(1);
            Ports::Write8(0x60, a_write);
        }

        uint8_t PS2Mouse::Read()
        {
            Wait(0);
            return Ports::Read8(0x60);
        }

        void PS2Mouse::SetPosition(uint32_t x, uint32_t y)
        {
            X = x;
            Y = y;
        }

        bool PS2Mouse::IsLeftPressed() { return LeftBtn; }

        bool PS2Mouse::IsRightPressed() { return RightBtn; }

        uint32_t PS2Mouse::GetX() { return X; }

        uint32_t PS2Mouse::GetY() { return Y; }
    }
}