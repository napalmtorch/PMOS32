#include <Kernel/HAL/Interrupts/IDT.h>
#include <Kernel/Core/Kernel.h>
using namespace System;

EXTC
{
    extern uint32_t _read_cr0();
    extern uint32_t _read_cr2();
    extern uint32_t _read_cr3();
    extern void _reload_segments();

    // ISR exception handler
    void _isr_handler(HAL::Registers32* regs)
    {
        asm volatile("cli");
        Core::Terminal.Clear();
        Debug::Error("ISR EXCEPTION - INT: 0x%8x, ERR: 0x%8x", regs->INT, regs->ERR);
        Debug::DumpRegisters(regs);
        asm volatile("hlt");
    }

    // IRQ handler
    void _irq_handler(HAL::Registers32* regs)
    {
        if (regs->INT >= 40) { HAL::Ports::Write8(0xA0, 0x20); }
        HAL::Ports::Write8(0x20, 0x20);

        if (HAL::IDT::Callbacks[regs->INT] != nullptr) 
        { 
            HAL::IDT::Callbacks[regs->INT](regs); 
        }
    }
}

namespace HAL
{
    namespace IDT
    {
        // Amount of entries
        #define IDT_ENTRIES 256

        // Entry array
        IDTGate Entries[IDT_ENTRIES];

        // Register
        IDTRegister IDTR;

        // Interrupt callback array
        InterruptCallback Callbacks[IDT_ENTRIES];

        // Initialize interrupt descriptor tables and service routines
        void Init()
        {
            memset(&Entries, 0, sizeof(IDTGate) * IDT_ENTRIES);
            memset(&Callbacks, 0, sizeof(InterruptCallback) * IDT_ENTRIES);

            Debug::Info("Setting IDT descriptors...");
            InitExceptions();

            Ports::Write8(0x20, 0x11);
            Ports::Write8(0xA0, 0x11);
            Ports::Write8(0x21, 0x20);
            Ports::Write8(0xA1, 0x28);
            Ports::Write8(0x21, 0x04);
            Ports::Write8(0xA1, 0x02);
            Ports::Write8(0x21, 0x01);
            Ports::Write8(0xA1, 0x01);
            Ports::Write8(0x21, 0x0);
            Ports::Write8(0xA1, 0x0); 
            Debug::Info("Re-mapped PIC controller");

            InitIRQs();

            Flush();
            Debug::OK("Initialized IDT");
        }

        // Initialize exception handler descriptors
        void InitExceptions()
        {
            SetDescriptor(0, (uint32_t)isr0);
            SetDescriptor(1, (uint32_t)isr1);
            SetDescriptor(2, (uint32_t)isr2);
            SetDescriptor(3, (uint32_t)isr3);
            SetDescriptor(4, (uint32_t)isr4);
            SetDescriptor(5, (uint32_t)isr5);
            SetDescriptor(6, (uint32_t)isr6);
            SetDescriptor(7, (uint32_t)isr7);
            SetDescriptor(8, (uint32_t)isr8);
            SetDescriptor(9, (uint32_t)isr9);
            SetDescriptor(10, (uint32_t)isr10);
            SetDescriptor(11, (uint32_t)isr11);
            SetDescriptor(12, (uint32_t)isr12);
            SetDescriptor(13, (uint32_t)isr13);
            SetDescriptor(14, (uint32_t)isr14);
            SetDescriptor(15, (uint32_t)isr15);
            SetDescriptor(16, (uint32_t)isr16);
            SetDescriptor(17, (uint32_t)isr17);
            SetDescriptor(18, (uint32_t)isr18);
            SetDescriptor(19, (uint32_t)isr19);
            SetDescriptor(20, (uint32_t)isr20);
            SetDescriptor(21, (uint32_t)isr21);
            SetDescriptor(22, (uint32_t)isr22);
            SetDescriptor(23, (uint32_t)isr23);
            SetDescriptor(24, (uint32_t)isr24);
            SetDescriptor(25, (uint32_t)isr25);
            SetDescriptor(26, (uint32_t)isr26);
            SetDescriptor(27, (uint32_t)isr27);
            SetDescriptor(28, (uint32_t)isr28);
            SetDescriptor(29, (uint32_t)isr29);
            SetDescriptor(30, (uint32_t)isr30);
            SetDescriptor(31, (uint32_t)isr31);
            Debug::Info("Set exception handler descriptors");
        }

        // Initialize IRQ descriptors
        void InitIRQs()
        {
            SetDescriptor(32, (uint32_t)irq0);
            SetDescriptor(33, (uint32_t)irq1);
            SetDescriptor(34, (uint32_t)irq2);
            SetDescriptor(35, (uint32_t)irq3);
            SetDescriptor(36, (uint32_t)irq4);
            SetDescriptor(37, (uint32_t)irq5);
            SetDescriptor(38, (uint32_t)irq6);
            SetDescriptor(39, (uint32_t)irq7);
            SetDescriptor(40, (uint32_t)irq8);
            SetDescriptor(41, (uint32_t)irq9);
            SetDescriptor(42, (uint32_t)irq10);
            SetDescriptor(43, (uint32_t)irq11);
            SetDescriptor(44, (uint32_t)irq12);
            SetDescriptor(45, (uint32_t)irq13);
            SetDescriptor(46, (uint32_t)irq14);
            SetDescriptor(47, (uint32_t)irq15);
            SetDescriptor(128, (uint32_t)irq128);
            Debug::Info("Set IRQ handler descriptors");
        }

        /// @brief Register interrupt handler callback @param irq Interrupt number @param callback Callback pointer
        void RegisterInterrupt(uint8_t irq, InterruptCallback callback)
        {
            Callbacks[irq] = callback;
            Debug::Info("Registered interrupt 0x%2x with callback 0x%8x", irq, (uint32_t)callback);
        }

        /// @brief Un-register interrupt handler callback @param irq Interrupt number
        void UnregisterInterrupt(uint8_t irq)
        {
            Callbacks[irq] = nullptr;
            Debug::Info("Unregistered interrupt 0x%2x", irq);
        }

        // Setup register and reload segments
        void Flush()
        {
            IDTR.Base = (uint32_t)&Entries;
            IDTR.Limit = IDT_ENTRIES * sizeof(IDTGate) - 1;
            _reload_segments();
            asm volatile("lidtl (%0)" : : "r" (&IDTR));
        }

        /// @brief Set IDT descriptor @param n Interrupt number @param handler Handler method pointer
        void SetDescriptor(int n, uint32_t handler)
        {
            Entries[n].LowOffset  = _low16(handler);
            Entries[n].Selector   = 0x08;
            Entries[n].AlwaysZero = 0;
            Entries[n].Flags      = 0x8E;
            Entries[n].HighOffset = _high16(handler);
        }
    }
}