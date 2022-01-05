#include <Kernel/HAL/Common/PIT.h>
#include <Kernel/Core/Kernel.h>
using namespace System;

// pit ports
#define PIT_CMD  0x43
#define PIT_DATA 0x40

namespace HAL
{
    namespace PIT
    {
        // Function pointer for interrupt callback
        InterruptCallback Callback;

        // Current frequency in hertz
        uint32_t Frequency;

        // Total amount of ticks
        uint64_t Ticks;

        /// @brief Initialize programmable interval timer @param freq Frequency value in hertz @param callback Interrupt callback pointer
        void Init(uint32_t freq, InterruptCallback callback)
        {
            // set callback
            Callback = callback;

            // set frequency variable
            if (freq == 0) { Disable(); }
            if (freq > 5000) { Debug::Info("Adjusted PIT frequency: %d Hz", Frequency); freq = 5000; }
            Frequency = freq;

            // calculate actual frequency
            uint32_t f = 1193180 / Frequency;
            uint8_t high = (uint8_t)((f & 0xFF00) >> 8);
            uint8_t low  = (uint8_t)((f & 0x00FF));

            // send frequency to pit
            Ports::Write8(PIT_CMD, 0x36);
            Ports::Write8(PIT_DATA, low);
            Ports::Write8(PIT_DATA, high);
            
            IDT::RegisterInterrupt(IRQ0, Callback);
            Debug::OK("Initialized PIT at %d Hz", Frequency);
        }

        // Disable programmable interval timer
        void Disable()
        {
            IDT::UnregisterInterrupt(IRQ0);
        }

        // Update programmable interval timer information
        void Calculate()
        {
            Ticks++;
        }

        /// @brief Get total amount of ticks @return Amount of ticks
        uint64_t GetTicks() { return Ticks; }

        /// @brief Get current frequency @return Frequency
        uint32_t GetFrequency() { return Frequency; }
        
    }
}