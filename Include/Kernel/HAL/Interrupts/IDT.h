#pragma once
#include <Lib/Types.h>

#define IRQ0 32
#define IRQ1 33
#define IRQ2 34
#define IRQ3 35
#define IRQ4 36
#define IRQ5 37
#define IRQ6 38
#define IRQ7 39
#define IRQ8 40
#define IRQ9 41
#define IRQ10 42
#define IRQ11 43
#define IRQ12 44
#define IRQ13 45
#define IRQ14 46
#define IRQ15 47
#define IRQ_SYSCALL 160

namespace HAL
{
    typedef struct
    {
        uint32_t DS;
        uint32_t EDI, ESI, EBP, ESP, EBX, EDX, ECX, EAX;
        uint32_t INT, ERR;
        uint32_t EIP, CS, EFLAGS, USERESP, SS;
    } Registers32;

    typedef void (*InterruptCallback)(Registers32* regs);

    namespace IDT
    {
        extern InterruptCallback Callbacks[];

        typedef struct
        {
            uint16_t LowOffset;
            uint16_t Selector;
            uint8_t  AlwaysZero;
            uint8_t  Flags;
            uint16_t HighOffset;
        } PACKED IDTGate;

        typedef struct
        {
            uint16_t Limit;
            uint32_t Base;
        } PACKED IDTRegister;

        void Init();
        void InitExceptions();
        void InitIRQs();
        void RegisterInterrupt(uint8_t irq, InterruptCallback callback);
        void UnregisterInterrupt(uint8_t irq);
        void Flush();
        void SetDescriptor(int n, uint32_t handler);
    }
}

EXTC
{
    extern void isr0();
    extern void isr1();
    extern void isr2();
    extern void isr3();
    extern void isr4();
    extern void isr5();
    extern void isr6();
    extern void isr7();
    extern void isr8();
    extern void isr9();
    extern void isr10();
    extern void isr11();
    extern void isr12();
    extern void isr13();
    extern void isr14();
    extern void isr15();
    extern void isr16();
    extern void isr17();
    extern void isr18();
    extern void isr19();
    extern void isr20();
    extern void isr21();
    extern void isr22();
    extern void isr23();
    extern void isr24();
    extern void isr25();
    extern void isr26();
    extern void isr27();
    extern void isr28();
    extern void isr29();
    extern void isr30();
    extern void isr31();

    extern void irq0();
    extern void irq1();
    extern void irq2();
    extern void irq3();
    extern void irq4();
    extern void irq5();
    extern void irq6();
    extern void irq7();
    extern void irq8();
    extern void irq9();
    extern void irq10();
    extern void irq11();
    extern void irq12();
    extern void irq13();
    extern void irq14();
    extern void irq15();
    extern void irq128();
}