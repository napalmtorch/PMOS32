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

        typedef int (*ThreadProtocol)(void* arg);

        class Thread;
        class ThreadManager;

        class Thread
        {
            friend class ThreadManager;

            public:
                HAL::Registers32 Registers;
                uint8_t*         Stack;
                uint32_t         StackSize;
                ThreadState      State;
                ThreadProtocol   Protocol;
                bool             Locked;
                uint32_t         ID;
                char             Name[64];

            public:
                void Init(char* name, uint32_t stack_size, ThreadProtocol protocol);
                static void Exit();
                void Lock();
                void Unlock();
                bool Start();
                bool Stop();
        };

        class ThreadManager
        {
            friend class Thread;

            public:
                Thread** Threads;
                uint32_t Index;
                uint32_t Count;
                uint32_t CountMax;
                uint32_t CurrentID;

            public:
                void Init();
                void Load(Thread* thread);
                void Unload(Thread* thread);
                void Terminate(Thread* thread);
                void Lock();
                void Unlock();
                void Schedule();
                Thread* Create(char* name, uint32_t stack_size, ThreadProtocol protocol);

            private:
                Thread* GetNextThread(bool inc);
        };
    }
}


EXTC
{
    extern System::Threading::Thread* _current_thread;
    extern System::Threading::Thread* _next_thread;
}