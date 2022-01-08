#include <Kernel/System/Threading.h>
#include <Kernel/Core/Kernel.h>
using namespace HAL;
using namespace Graphics;

EXTC
{
    System::Threading::Thread* _current_thread;
    System::Threading::Thread* _next_thread;
    bool                       _threading_initialized;
    extern void _switch_thread();

    void _update_current_thread()
    {
        _current_thread = _next_thread;
    }

    void lock()
    {
        if (_current_thread == nullptr) { return; }
        _current_thread->Lock();
    }

    void unlock()
    {
        if (_current_thread == nullptr) { return; }
        _current_thread->Unlock();
    }
}

namespace System
{
    namespace Threading
    {
        uint32_t CurrentID = 0;

        void Thread::Init(char* name, uint32_t stack_size, ThreadProtocol protocol)
        {
            strcpy(Name, name);
            memset(&Registers, 0, sizeof(ThreadRegisters));
            StackSize = Core::PMM.Align(stack_size);
            Stack     = (uint8_t*)Core::Heap.Allocate(StackSize, true, Memory::HeapType::ThreadStack);
            Protocol  = protocol;
            ID        = CurrentID++;
            State     = ThreadState::Halted;

            uint32_t* stk = (uint32_t*)((uint32_t)Stack + (StackSize - sizeof(Registers32) - 16));
            *--stk = (uint32_t)this;
            *--stk = (uint32_t)0;
            *--stk = (uint32_t)Exit;
            *--stk = (uint32_t)protocol;
        
            Registers.EFLAGS = 0x202;
            Registers.EIP    = (uint32_t)Protocol;
            Registers.ESP    = (uint32_t)stk;
        }

        Thread* Thread::Create(char* name, uint32_t stack_size, ThreadProtocol protocol)
        {
            Thread* t = (Thread*)Core::Heap.Allocate(sizeof(Thread), true, Memory::HeapType::Thread);
            t->Init(name, stack_size, protocol);
            Debug::Info("Created thread - ID: %d, PROTOCOL: 0x%8x, NAME: %s", t->ID, (uint32_t)t->Protocol, t->Name);
            return t;
        }

        void Thread::Exit()
        {
            register int eax asm("eax");
            uint32_t code = eax;

            Debug::Info("Thread exited with code %d", code);
            _current_thread->State = ThreadState::Terminated;
            Core::ProcessMgr.Schedule();
            
            while (true) { Core::ProcessMgr.Schedule(); }
        }

        void Thread::Lock() { Locked = true; }
        void Thread::Unlock() { Locked = false; }

        bool Thread::Start()
        {
            return false;
        }

        bool Thread::Stop()
        {
            State = ThreadState::Terminated;
            return false;
        }
    }
}