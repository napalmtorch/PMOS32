#include <Kernel/System/Process.h>
#include <Kernel/Core/Kernel.h>
using namespace HAL;
using namespace Graphics;

EXTC
{
    extern void _switch_thread();
}

namespace System
{
    uint32_t ProcessID = 0;

    void Process::Init(char* name, Threading::ThreadProtocol protocol)
    {
        ThreadCountMax = 256;
        ThreadCount    = 1;
        ID             = ProcessID++;
        Threads = (Threading::Thread**)Core::Heap.Allocate(sizeof(Threading::Thread*) * ThreadCountMax, true, Memory::HeapType::PointerArray);

        char thread_name[128];
        memset(thread_name, 0, 128);
        strcpy(thread_name, name);
        strcat(thread_name, "_thread0");
        Threads[0] = Core::ThreadMgr.Create(thread_name, 0x10000, protocol);
        strcpy(Name, name);
    }

    Process* Process::Create(char* name, Threading::ThreadProtocol protocol)
    {
        Process* proc = (Process*)Core::Heap.Allocate(sizeof(Process), true, Memory::HeapType::Process);
        proc->Init(name, protocol);
        Debug::Info("Created process - ID: %d, NAME: '%s'", proc->ID, proc->Name);
        return proc;
    }

    Process* Process::CreateELF(char* name, uint8_t* data, uint32_t size)
    {
        Process* proc = Create(name, nullptr);
        Core::Heap.Free(proc->Threads[0]->Stack);
        Core::Heap.Free(proc->Threads[0]);

        Threading::Thread* t = (Threading::Thread*)Core::Heap.Allocate(sizeof(Threading::Thread), true, Memory::HeapType::Thread);
        proc->Threads[0] = t;
        ELF::Init(data, size, proc);
        
        strcpy(t->Name, name);
        strcat(t->Name, "_thread0");
        memset(&t->Registers, 0, sizeof(Registers32));
        t->StackSize = Core::PMM.Align(t->StackSize);
        t->Stack     = (uint8_t*)Core::Heap.Allocate(0x10000, true, Memory::HeapType::ThreadStack);
        t->ID        = Core::ThreadMgr.CurrentID++;
        t->State     = Threading::ThreadState::Halted;

        uint32_t* stk = (uint32_t*)((uint32_t)t->Stack + (t->StackSize - 16));
        *--stk = (uint32_t)t;
        *--stk = (uint32_t)0;
        *--stk = (uint32_t)Threading::Thread::Exit;
        *--stk = (uint32_t)t->Protocol;

        t->Registers.CS     = 0x08;
        t->Registers.DS     = 0x10;
        t->Registers.EFLAGS = 0x202;
        t->Registers.EIP    = (uint32_t)t->Protocol;
        t->Registers.ESP    = (uint32_t)stk;

        Debug::Info("Created ELF process - ID: %d, PROTOCOL: 0x%8x, NAME: '%s'", proc->ID, t->Protocol, proc->Name);
        return proc;
    }

    Process* Process::CreateKernel()
    {
        Process* proc = (Process*)Core::Heap.Allocate(sizeof(Process), true, Memory::HeapType::Process);
        proc->ThreadCountMax = 4096;
        proc->ThreadCount    = 1;
        proc->Threads = (Threading::Thread**)Core::Heap.Allocate(sizeof(Threading::Thread*) * proc->ThreadCountMax, true, Memory::HeapType::PointerArray);
        proc->ID = 0;
        proc->Running = true;
        ProcessID = 1;
        strcpy(proc->Name, "kernel");

        Threading::Thread* kthread = (Threading::Thread*)Core::Heap.Allocate(sizeof(Threading::Thread), true, Memory::HeapType::Thread);
        strcpy(kthread->Name, "kernel_thread0");
        kthread->ID        = 0;
        kthread->StackSize = 0;
        kthread->Stack     = nullptr;
        kthread->Protocol  = nullptr;
        kthread->Locked    = false;
        Debug::Info("Created kernel thread  - ID: %d, NAME: '%s'", kthread->ID, kthread->Name);

        proc->Threads[0] = kthread;
        Debug::Info("Created kernel process - ID: %d, NAME: '%s'", proc->ID, proc->Name);;
        return proc;
    }

    bool Process::Start()
    {
        Running = true;
        return true;
    }

    bool Process::Terminate()
    {
        Running = false;
        return false;
    }

    bool Process::LoadThread(Threading::Thread* thread)
    {
        if (thread == nullptr) { Debug::Error("Tried to load null thread in process '%s'", Name); return false; }
        int i = GetFreeIndex();
        if (i < 0 || i >= ThreadCountMax) { Debug::Error("Maximum amount of threads reached in process '%s'", Name); return false; }
        Threads[i] = thread;
        Threads[i]->State = Threading::ThreadState::Running;
        ThreadCount++;
        Debug::Info("Loaded thread '%s' in process '%s'", thread->Name, Name);
        return true;
    }

    bool Process::UnloadThread(Threading::Thread* thread)
    {
        if (thread == nullptr) { Debug::Error("Tried to unload null thread in process '%s'", Name); return false; }
        for (uint32_t i = 0; i < ThreadCountMax; i++)
        {
            if (Threads[i] == thread)
            {
                Debug::Info("Unloaded thread - ID: %d, NAME: '%s'", thread->ID, thread->Name);
                Core::VMM.KernelDirectory.Unmap((void*)Virtual);
                Core::Heap.Free(Threads[i]->Stack);
                Core::Heap.Free(Threads[i]);
                Threads[i] = nullptr;
                ThreadCount--;
                return true;
            }
        }

        return false;
    }

    bool Process::StartThread(Threading::Thread* thread)
    {
        thread->State = Threading::ThreadState::Running;
    }

    bool Process::TerminateThread(Threading::Thread* thread)
    {
        thread->State = Threading::ThreadState::Terminated;
    }

    int Process::GetFreeIndex()
    {
        for (uint32_t i = 0; i < ThreadCountMax; i++)
        {
            if (Threads[i] == nullptr) { return (int)i; }
        }
        return -1;
    }

    // --------------------------------------------------------------------------------------

    void ProcessManager::Init()
    {
        Count    = 1;
        CountMax = 256;
        Index    = 0;
        Processes = (Process**)Core::Heap.Allocate(sizeof(Process*) * CountMax, true, Memory::HeapType::PointerArray);

        Process* kproc = Process::CreateKernel();
        Processes[0] = kproc;
        CurrentProc = kproc;
        NextProc = nullptr;

        Core::ThreadMgr.CurrentID = 1;

        Debug::OK("Initialized process manager");
    }

    void ProcessManager::Print()
    {
        Debug::Write("------ ", Color4::Gray);
        Debug::Write("PROCESSES", Color4::Green);
        Debug::Write(" ---------------------------------------------\n", Color4::Gray);
        Debug::WriteLine(" ID          ADDR        STATE   THREADS   NAME", Color4::Magenta);

        for (uint32_t i = 0; i < CountMax; i++)
        {
            if (Processes[i] == nullptr) { continue; }
            Debug::PrintFormatted(" 0x%8x  0x%8x  0x%2x    %d         %s\n", 
                                    Processes[i]->ID, (uint32_t)Processes[i], (uint32_t)Processes[i]->Running, Processes[i]->ThreadCount, Processes[i]->Name);
        }

        Debug::WriteLine("--------------------------------------------------------------\n", Color4::Gray);
    }

    bool ProcessManager::Load(Process* proc)
    {
        if (proc == nullptr) { Debug::Error("Tried to load null process"); return false; }
        int i = GetFreeIndex();
        if (i < 0 || i >= CountMax) { Debug::Error("Maximum amount of processes reached"); return false; }
        Processes[i] = proc;
        Processes[i]->Running = true;
        Count++;
        Debug::Info("Loaded process - ID: %d, NAME: '%s'", proc->ID, proc->Name);
        return true;
    }

    bool ProcessManager::Unload(Process* proc)
    {
        if (proc == nullptr) { Debug::Error("Tried to unload null process"); return false; }
        for (uint32_t i = 0; i < CountMax; i++)
        {
            if (Processes[i] == proc)
            {
                Core::Heap.Free(Processes[i]->ProgramData);
                Core::Heap.Free(Processes[i]->Threads);
                Debug::Info("Unloaded process '%s'", Processes[i]->Name);
                Core::Heap.Free(Processes[i]);
                Processes[i] = nullptr;
                Count--;
                return true;
            }
        }
        return false;
    }

    bool switch_proc = false;

    void ProcessManager::Schedule()
    {
        switch_proc = false;

        // get current process
        CurrentProc = Processes[Index];
        if (CurrentProc == nullptr) { return; }

        // get current thread in process
        _current_thread = CurrentProc->Threads[CurrentProc->Index];
        if (_current_thread == nullptr) { switch_proc = true; }
        if (_current_thread->Locked && _current_thread->State == Threading::ThreadState::Running) { return; }

        // get next thread in process
        if (!switch_proc)
        {
            int old_index = CurrentProc->Index;
            _next_thread = GetNextThread(CurrentProc, true);
            if (_next_thread == nullptr) { Debug::Error("Next thread was null"); return; }
            if (_next_thread != _current_thread) { CurrentProc->Switched++; }
            if (CurrentProc->Index == old_index) { switch_proc = true; }
        }

        if (CurrentProc->Switched >= CurrentProc->ThreadCount)
        {
            CurrentProc->Switched = 0;
            switch_proc = true;
        }

        if (_next_thread->State == Threading::ThreadState::Terminated)
        {
            CurrentProc->UnloadThread(_next_thread);
            switch_proc = true;
        }

        if (switch_proc)
        {
            NextProc = GetNextProcess(true);
            if (NextProc == nullptr) { Debug::Error("Next process was null"); return; }
            if (NextProc != CurrentProc) { _next_thread = NextProc->Threads[0]; }
            CurrentProc = NextProc;
        }
        else { NextProc = CurrentProc; }

        _switch_thread();
    }

    Process* ProcessManager::GetNextProcess(bool inc)
    {
        if (inc) { Index++; }
        while (true)
        {
            if (Index >= CountMax) { Index = 0; break; }
            if (Processes[Index] == nullptr) { Index++; continue; }
            if (!Processes[Index]->Running) { Unload(Processes[Index]); Index++; continue; }
            if (Processes[Index]->Running) 
            { 
                if (Processes[Index]->ThreadCount > 0) { break; }
                else { Processes[Index]->Running = false; Index++; continue; } 
            }
        }
        return Processes[Index];
    }

    Threading::Thread* ProcessManager::GetNextThread(Process* proc, bool inc)
    {
        int old = proc->Index;
        if (inc) { proc->Index++; }
        while (true)
        {
            if (proc->Index >= proc->ThreadCountMax) { proc->Index = 0; break; }
            if (proc->Threads[proc->Index] == nullptr) { proc->Index++; continue; }
            if (proc->Threads[proc->Index]->State == Threading::ThreadState::Running) { break; }
        }
        if (proc->Index == old) { return proc->Threads[old]; }
        return proc->Threads[proc->Index];
    }

    int ProcessManager::GetFreeIndex()
    {
        for (uint32_t i = 0; i < CountMax; i++)
        {
            if (Processes[i] == nullptr) { return i; }
        }
        return -1;
    }
}