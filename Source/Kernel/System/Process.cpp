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
        MessageCount   = 0;
        Threads = (Threading::Thread**)Core::Heap.Allocate(sizeof(Threading::Thread*) * ThreadCountMax, true, Memory::HeapType::PointerArray);
        Messages = (ProcessMessage*)Core::Heap.Allocate(sizeof(ProcessMessage) * 256, true, Memory::HeapType::Array);

        char thread_name[128];
        memset(thread_name, 0, 128);
        strcpy(thread_name, name);
        strcat(thread_name, "_thread0");
        Threads[0] = Threading::Thread::Create(thread_name, 0x10000, protocol);
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

        uint32_t phys = Core::VMM.AllocateDirectory();

        Core::VMM.KernelDirectory.Map(phys, phys);
        memset((void*)phys, 0, 0x2000);

        proc->PageDir = (Memory::PageDirectory*)Core::Heap.Allocate(sizeof(Memory::PageDirectory), true, Memory::HeapType::Array);
        proc->PageDir->Init(phys);

        proc->PageDir->Map(0, 0);
        proc->PageDir->Map(0x00400000, 0x00400000);
        proc->PageDir->Map(0x00800000, 0x00800000);
        proc->PageDir->Map(0x00C00000, 0x00C00000);
        proc->PageDir->Map(0xC0000000, 0);
        proc->PageDir->Map(0xFD000000, 0xFD000000);
        proc->PageDir->Map(phys, phys);

        // calculate page count
        uint32_t ps = 4 * 1024 * 1024;
        uint32_t pages = Core::Heap.Size / ps;
        uint32_t v = Core::Heap.Virtual, p = Core::Heap.Physical;

        // map pages
        Debug::Info("Mapping pages for heap = PAGES: %d, SIZE: %d MB", pages, Core::Heap.Size / 1024 / 1024);
        for (uint32_t i = 0; i < pages; i++)
        {
            proc->PageDir->Map(p, p, false);
            proc->PageDir->Map(v, p, false);
            v += ps;
            p += ps;
        }
        Debug::Info("Mapped kernel heap onto process '%s'", name);

        ELF::Init(data, size, proc);
        
        strcpy(t->Name, name);
        strcat(t->Name, "_thread0");
        memset(&t->Registers, 0, sizeof(Threading::ThreadRegisters));
        t->StackSize = 512 * 1024;
        t->Stack     = (uint8_t*)Core::Heap.Allocate(512 * 1024, true, Memory::HeapType::ThreadStack);
        t->ID        = Threading::CurrentID++;
        t->State     = Threading::ThreadState::Halted;

        uint32_t* stk = (uint32_t*)((uint32_t)t->Stack + (t->StackSize - 16));
        *--stk = (uint32_t)t;
        *--stk = (uint32_t)0;
        *--stk = (uint32_t)Threading::Thread::Exit;
        *--stk = (uint32_t)t->Protocol;

        t->Registers.EFLAGS = 0x202;
        t->Registers.EIP    = (uint32_t)t->Protocol;
        t->Registers.ESP    = (uint32_t)stk;
        t->Registers.CR3    = proc->PageDir->GetPhysical();

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
        kthread->Registers.CR3 = Core::VMM.KernelDirectory.GetPhysical();
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
        return true;
    }

    bool Process::IsMessageReady() { return MessageCount > 0; }

    bool Process::PushMessage(ProcessMessage msg)
    {
        if (MessageCount >= 256) { Debug::Error("Maximuma mount of messages reached on process '%s'", Name); return false; }
        Messages[MessageCount] = msg;
        MessageCount++;
        Debug::Info("Pushed message onto process '%s'", Name);
        return true;
    }

    ProcessMessage Process::PopMessage()
    {
        if (MessageCount < 0) { return { 0 }; }
        MessageCount--;
        ProcessMessage msg = Messages[MessageCount];
        Debug::Info("Popped message from process '%s'", Name);
        return msg;
    }

    bool Process::LoadThread(Threading::Thread* thread)
    {
        if (thread == nullptr) { Debug::Error("Tried to load null thread in process '%s'", Name); return false; }
        int i = GetFreeIndex();
        if (i < 0 || i >= ThreadCountMax) { Debug::Error("Maximum amount of threads reached in process '%s'", Name); return false; }
        Threads[i] = thread;
        if (ID == 0) { Threads[i]->Registers.CR3 = Core::VMM.KernelDirectory.GetPhysical(); }
        else { Threads[i]->Registers.CR3 = PageDir->GetPhysical(); }
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

        Threading::CurrentID = 1;

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

    void ProcessManager::PrintThreads()
    {
        Debug::Write("------ ", Color4::Gray);
        Debug::Write("THREADS", Color4::Green);
        Debug::Write(" -----------------------------------------------\n", Color4::Gray);
        Debug::WriteLine(" ID          ADDR        STATE        NAME", Color4::Magenta);

        for (uint32_t i = 0; i < CountMax; i++)
        {
            if (Processes[i] == nullptr) { continue; }
            for (uint32_t j = 0; j < Processes[i]->ThreadCountMax; j++)
            {
                if (Processes[i]->Threads[j] == nullptr) { continue; }

                Debug::PrintFormatted(" 0x%8x  0x%8x  0x%2x         %s\n", 
                        Processes[i]->Threads[j]->ID, (uint32_t)Processes[i]->Threads[j], (uint32_t)Processes[i]->Threads[j]->State, Processes[i]->Threads[j]->Name);
            }
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
                for (uint32_t j = 0; j < Processes[i]->ThreadCountMax; j++)
                {
                    if (Processes[i]->Threads[j] == nullptr) { continue; }
                    Processes[i]->Threads[j]->Stop();
                    proc->UnloadThread(Processes[i]->Threads[j]);
                }

                Debug::Info("Unloaded process '%s'", Processes[i]->Name);
                Core::Heap.Free(Processes[i]->ProgramData);
                Core::Heap.Free(Processes[i]->Threads);
                Core::VMM.FreeDirectory(Processes[i]->PageDir->GetPhysical());
                Core::VMM.KernelDirectory.Unmap((void*)Processes[i]->PageDir->GetPhysical());
                Core::Heap.Free(Processes[i]->PageDir);
                Core::Heap.Free(Processes[i]->Messages);
                Core::Heap.Free(Processes[i]);
                Processes[i] = nullptr;
                Count--;
                Debug::Info("Finished unloading process");
                return true;
            }
        }
        return false;
    }

    bool ProcessManager::Kill(Process* proc)
    {
        if (proc == nullptr) { return false; }
        for (uint32_t i = 0; i < CountMax; i++)
        {
            if (Processes[i] == nullptr) { continue; }
            if (i == 0) { continue; }
            if (Processes[i] == proc) { return Processes[i]->Terminate(); }
        }
        return false;
    }

    bool ProcessManager::Kill(char* name)
    {
        if (name == nullptr) { return false; }
        for (uint32_t i = 0; i < CountMax; i++)
        {
            if (Processes[i] == nullptr) { continue; }
            if (i == 0) { continue; }
            if (streql(Processes[i]->Name, name)) { return Processes[i]->Terminate(); }
        }
        return false;
    }

    bool ProcessManager::Kill(int index)
    {
        if (index < 1 || index >= CountMax) { return false; }
        for (uint32_t i = 0; i < CountMax; i++)
        {
            if (Processes[i] == nullptr) { continue; }
            if (i == 0) { continue; }
            if (i == index) { return Processes[i]->Terminate(); }
        }
        return false;
    }

    bool ProcessManager::SendMessage(char* name, ProcessMessage msg)
    {
        if (name == nullptr) { return false; }
        if (strlen(name) == 0) { return false; }

        for (uint32_t i = 0; i < CountMax; i++)
        {
            if (Processes[i] == nullptr) { continue; }
            if (streql(Processes[i]->Name, name))
            {
                Processes[i]->PushMessage(msg);
                Debug::Info("Sent %d byte message to process '%s'", msg.Size, name);
                return true;
            }
        }

        return false;
    }

    bool switch_proc = false;
    int old_index = 0;
    void ProcessManager::Schedule()
    {
        switch_proc = false;

        // get current process
        CurrentProc = Processes[Index];

        // get current thread in process
        _current_thread = CurrentProc->Threads[CurrentProc->Index];
        if (_current_thread == nullptr) { switch_proc = true; }
        if (_current_thread->Locked && _current_thread->State == Threading::ThreadState::Running) { return; }

        // get next thread in process
        if (!switch_proc)
        {
            old_index = CurrentProc->Index;
            _next_thread = GetNextThread(CurrentProc, true);
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
            if (!Processes[Index]->Running) 
            {
                Unload(Processes[Index]); 
                Index++; 
                continue; 
            }
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