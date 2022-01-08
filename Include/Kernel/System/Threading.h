#pragma once
#include <Lib/Types.h>
#include <Kernel/HAL/Interrupts/IDT.h>

EXTC
{
    void lock();
    void unlock();
}

namespace System
{
    namespace Threading
    {
        enum class ThreadState : uint8_t
        {
            Halted,
            Running,
            Terminated,
        };

        typedef struct
        {
            uint32_t ESP, EBP, EDI, ESI;
            uint32_t EAX, EBX, ECX, EDX;
            uint32_t EIP, CR3, EFLAGS, PADDING;
        } PACKED ThreadRegisters;

        typedef int (*ThreadProtocol)(void* arg);

        extern uint32_t CurrentID;

        class Thread
        {
            public:
                ThreadRegisters  Registers;
                uint8_t*         Stack;
                uint32_t         StackSize;
                ThreadState      State;
                ThreadProtocol   Protocol;
                bool             Locked;
                uint32_t         ID;
                char             Name[64];

            public:
                void Init(char* name, uint32_t stack_size, ThreadProtocol protocol);
                static Thread* Create(char* name, uint32_t stack_size, ThreadProtocol protocol);
                static void Exit();
                void Lock();
                void Unlock();
                bool Start();
                bool Stop();
        };
    }
}


EXTC
{
    extern System::Threading::Thread* _current_thread;
    extern System::Threading::Thread* _next_thread;
}