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
        void Thread::Init(char* name, uint32_t stack_size, ThreadProtocol protocol)
        {
            strcpy(Name, name);
            memset(&Registers, 0, sizeof(Registers32));
            StackSize = Core::PMM.Align(stack_size);
            Stack     = (uint8_t*)Core::Heap.Allocate(StackSize, true, Memory::HeapType::ThreadStack);
            Protocol  = protocol;
            ID        = Core::ThreadMgr.CurrentID++;
            State     = ThreadState::Halted;

            uint32_t* stk = (uint32_t*)((uint32_t)Stack + (StackSize - 16));
            *--stk = (uint32_t)this;
            *--stk = (uint32_t)0;
            *--stk = (uint32_t)Exit;
            *--stk = (uint32_t)protocol;

            Registers.CS     = 0x08;
            Registers.DS     = 0x10;
            Registers.EFLAGS = 0x200;
            Registers.EIP    = (uint32_t)Protocol;
            Registers.ESP    = (uint32_t)stk;
        }

        void Thread::Exit()
        {
            register int eax asm("eax");
            uint32_t code = eax;

            lock();
            Debug::Info("Thread exited with code %d", code);
            _current_thread->State = ThreadState::Terminated;
            unlock();
            
            while (true);
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

        void ThreadManager::Init()
        {
            if (_threading_initialized) { Debug::Error("Thread manager already initialized"); return; }

            CountMax  = 256;
            Count     = 0;
            Threads   = (Thread**)Core::Heap.Allocate(sizeof(Thread*) * CountMax, true, Memory::HeapType::PointerArray);
            CurrentID = 1;
            Index     = 0;

            Thread* kthread = (Thread*)Core::Heap.Allocate(sizeof(Thread), true, Memory::HeapType::Thread);
            strcpy(kthread->Name, "kernel");
            kthread->ID        = 0;
            kthread->StackSize = 0;
            kthread->Stack     = nullptr;
            kthread->Protocol  = nullptr;
            kthread->Locked    = false;

            _threading_initialized = true;
            _current_thread = kthread;

            Load(kthread);
        }

        void ThreadManager::Load(Thread* thread)
        {
            if (thread == nullptr) { Debug::Error("Tried to load null thread"); return; }
            Threads[Count] = thread;
            thread->State = ThreadState::Running;
            Count++;
            Debug::Info("Loaded thread - ID: %d, NAME: %s", thread->ID, thread->Name);
        }

        void ThreadManager::Unload(Thread* thread)
        {
            thread->State = ThreadState::Terminated;
        }

        void ThreadManager::Terminate(Thread* thread)
        {
            thread->State = ThreadState::Terminated;
        }

        void ThreadManager::Lock() 
        { 
            if (_current_thread == nullptr) { return; }
            _current_thread->Locked = true; 
        }
        void ThreadManager::Unlock() 
        { 
            if (_current_thread == nullptr) { return; }
            _current_thread->Locked = false; 
        }

        void ThreadManager::Schedule()
        {
            _current_thread = Threads[Index];
            if (_current_thread == nullptr) { return; }

            _next_thread = GetNextThread(true);
            if (_next_thread == nullptr) { Debug::Warning("Next thread was null - skipping"); return; }      

            _switch_thread();
        }

        Thread* ThreadManager::GetNextThread(bool inc)
        {
            if (inc) { Index++; }
            while (true)
            {
                if (Index >= Count) { Index = 0; break; }
                if (Threads[Index] == nullptr) { Index++; continue; }
                if (Threads[Index]->State == ThreadState::Running) { break; }
            }
            return Threads[Index];
        }

        Thread* ThreadManager::Create(char* name, uint32_t stack_size, ThreadProtocol protocol)
        {
            Thread* t = (Thread*)Core::Heap.Allocate(sizeof(Thread), true, Memory::HeapType::Thread);
            t->Init(name, stack_size, protocol);
            Debug::Info("Created thread - ID: %d, PROTOCOL: 0x%8x, NAME: %s", t->ID, (uint32_t)t->Protocol, t->Name);
            return t;
        }
    }
}