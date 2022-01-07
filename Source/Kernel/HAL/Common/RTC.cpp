#include <Kernel/HAL/Common/RTC.h>
#include <Kernel/Core/Kernel.h>
using namespace System;

#define RTC_PORT_CMD  0x70
#define RTC_PORT_DATA 0x71

namespace HAL
{
    namespace RTC
    {
        DateTime CurrentTime;
        uint32_t Ticks, UpdateTicks;
        uint32_t Frequency;

        void WriteRegister(uint16_t reg, uint8_t data);
        uint8_t ReadRegister(uint16_t reg);
        bool IsUpdating();

        void Init()
        {
            memset(&CurrentTime, 0, sizeof(DateTime));
            Ticks = 0;
            Frequency = 8192;

            // send initialization data to ports
            Ports::Write8(0x70, 0x8B);		    // select register B, and disable NMI
            char prev = Ports::Read8(0x71);	    // read the current value of register B
            Ports::Write8(0x70, 0x8B);		    // set the index again (a read will reset the index to register D)
            Ports::Write8(0x71, prev | 0x40);	// write the previous value ORed with 0x40. This turns on bit 6 of register B

            Ports::Write8(0x70, 0x0C);      	// select register C
            (void)Ports::Read8(0x71);           // dispose contents

            uint32_t rate = 3;

            rate &= 0x0F;			// rate must be above 2 and not over 15
            Ports::Write8(0x70, 0x8A);		// set index to register A, disable NMI
            prev = Ports::Read8(0x71);	// get initial value of register A
            Ports::Write8(0x70, 0x8A);		// reset index to A
            Ports::Write8(0x71, (prev & 0xF0) | rate); //write only our rate to A. Note, rate is the bottom 4 bits.

            Fetch();

            IDT::RegisterInterrupt(IRQ8, Callback);

            Debug::Info("Initialized RTC controller");
        }

        void Fetch()
        {
            // fetch time
            CurrentTime.Second = ReadRegister(0x00);
            CurrentTime.Minute = ReadRegister(0x02);
            CurrentTime.Hour   = ReadRegister(0x04);

            // fetch date
            CurrentTime.Day    = ReadRegister(0x07);
            CurrentTime.Month  = ReadRegister(0x08);
            CurrentTime.Year   = ReadRegister(0x09);

            // check if time is binary coded decimal
            uint8_t bcd = ReadRegister(0x0B);

            // convert from binary coded decimal if required
            if (!(bcd & 0x04))
            {
                CurrentTime.Second  = (CurrentTime.Second & 0x0F) + (CurrentTime.Second / 16) * 10;
                CurrentTime.Minute  = (CurrentTime.Minute & 0x0F) + (CurrentTime.Minute / 16) * 10;
                CurrentTime.Hour    = ((CurrentTime.Hour & 0x0F) + (CurrentTime.Hour / 16) * 10) | (CurrentTime.Hour & 0x80);
                CurrentTime.Day     = (CurrentTime.Day & 0x0F) + (CurrentTime.Day / 16) * 10;
                CurrentTime.Month   = (CurrentTime.Month & 0x0F) + (CurrentTime.Month / 16) * 10;
                CurrentTime.Year    = (CurrentTime.Year & 0x0F) + (CurrentTime.Year / 16) * 10;
            }
        }

        void Callback(Registers32* regs)
        {
            Ticks++;
            UpdateTicks++;

            if (Ticks >= 7)
            { 
                CurrentTime.Millisecond++; 
                Ticks = 0; 
            }

            if (UpdateTicks >= Frequency / 2)
            {
                Fetch();
                UpdateTicks = 0;
            }

            if (CurrentTime.Millisecond >= 1000) 
            { 
                CurrentTime.Millisecond = 0; 
            }

            Ports::Write8(0x70, 0x0C);
            (void)Ports::Read8(0x71);
        }

        void WriteRegister(uint16_t reg, uint8_t data)
        {
            if (!IsUpdating())
            {
                Ports::Write8(RTC_PORT_CMD, reg);
                Ports::Write8(RTC_PORT_DATA, data);
            }
        }

        uint8_t ReadRegister(uint16_t reg)
        {
            Ports::Write8(RTC_PORT_CMD, reg);
            return Ports::Read8(RTC_PORT_DATA);
        }

        bool IsUpdating()
        {
            Ports::Write8(RTC_PORT_CMD, 0x0A);
            uint8_t status = HAL::Ports::Read8(RTC_PORT_DATA);
            return (bool)(status & 0x80);
        }

        DateTime GetCurrentTime() { return CurrentTime; }
    }
}