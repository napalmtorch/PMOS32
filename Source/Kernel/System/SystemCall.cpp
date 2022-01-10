#include <Kernel/System/SystemCall.h>
#include <Kernel/Core/Kernel.h>
using namespace HAL;
using namespace Graphics;

namespace System
{
    namespace SystemCalls
    {
        SystemCall SystemCalls[256];
        SystemCallArguments Queue[256];
        uint32_t   Count;
        uint32_t   QueueIndex;

        void Init()
        {
            for (uint32_t i = 0; i < 256; i++) { Queue[i] = { 0, 0, 0, 0 }; }

            QueueIndex = 0;
            Count = 0;

            Register(SystemCalls::PRINT);
            Register(SystemCalls::MTABLE);
            Register(SystemCalls::EXEC);
            Register(SystemCalls::LOCK);
            Register(SystemCalls::UNLOCK);
            Register(SystemCalls::CLISTATE);
            Register(SystemCalls::EXIT);
            Register(SystemCalls::YIELD);
            Register(SystemCalls::CMD);

            IDT::RegisterInterrupt(IRQ_SYSCALL, Callback);

            Debug::Info("Initialized system calls");
        }

        void Register(SystemCall call)
        {
            if (Count >= 256) { Debug::Error("Maximum amount of system calls reached"); return; }
            SystemCalls[Count] = call;
            Debug::Info("Registered system call - CODE: 0x%8x, NAME: '%s'", call.Code, call.Name);
            Count++;
        }

        void Monitor()
        {
            while (QueueIndex > 0)
            {
                SystemCallArguments args = Pop();
                Execute(args);
            }
        }

        void Callback(HAL::Registers32* regs)
        {
            SystemCallArguments args = { regs->EAX, (void*)regs->EBX, regs->ECX, (void*)regs->EDX };

            if (args.Code == SystemCalls::YIELD.Code || args.Code == SystemCalls::MTABLE.Code || args.Code == SystemCalls::CLISTATE.Code || args.Code == SystemCalls::YIELD.Code ||
                args.Code == SystemCalls::CMD.Code) { Execute(args); }
            else { Push(args); }
        }

        void Execute(HAL::Registers32* regs)
        {
            SystemCallArguments args = { regs->EAX, (void*)regs->EBX, regs->ECX, (void*)regs->EDX };
            Execute(args);
        }

        void Execute(SystemCallArguments args)
        {
            for (uint32_t i = 0; i < 256; i++)
            {
                if (SystemCalls[i].Handler == nullptr) { continue; }
                if (SystemCalls[i].Code == args.Code)
                {
                    //Debug::Info("EXEC SYSTEM CALL 0x%8x, 0x%8x, 0x%8x, 0x%8x", args.Argument, (uint32_t)args.Source, args.Code, (uint32_t)args.Destination);
                    SystemCalls[i].Handler(args);
                    return;
                }
            }
            Debug::Error("Invalid system call 0x%8x", args.Code);
        }

        bool Push(SystemCallArguments args)
        {
            if (QueueIndex >= 256) { return false; }
            Queue[QueueIndex] = args;
            QueueIndex++;
            Debug::Info("Pushed system call 0x%8x", args.Code);
            return true;
        }

        SystemCallArguments Pop()
        {
            if (QueueIndex < 0) { return { 0, 0, 0, 0 }; }
            QueueIndex--;
            SystemCallArguments args = Queue[QueueIndex];
            //Debug::Info("Popped system call 0x%8x", args.Code);
            return args;
        }
    }

    namespace SystemCallMethods
    {
        void PRINT(SystemCallArguments args)
        {
            Debug::Write((char*)args.Source, DebugMode::All);
        }

        void MTABLE(SystemCallArguments args)
        {
            uint32_t* output_ptr = (uint32_t*)args.Destination;
            uint32_t  mtable     = (uint32_t)MethodLibrary::Entries;
            output_ptr[0] = mtable;
            Debug::Info("Grabbed method table");
        }

        void EXEC(SystemCallArguments args)
        {
            FileSystem::FSFile file = Core::FileSysHDD.ReadFile((char*)args.Source);
            if (file.Data == nullptr) { Debug::Error("Unable to locate file '%s'", DebugMode::All, (char*)args.Source); return; }
            Process* proc = Process::CreateELF(file.Name, file.Data, file.Size);
            Core::ProcessMgr.Load(proc);
        }

        void LOCK(SystemCallArguments args)
        {
            lock();
        }

        void UNLOCK(SystemCallArguments args)
        {
            unlock();
        }

        void CLISTATE(SystemCallArguments args)
        {
            uint32_t state = args.Argument;
            if (state == 1) { Core::CLI.Enable(); }
            else { Core::CLI.Disable(); }
        }

        void EXIT(SystemCallArguments args)
        {
            Core::ProcessMgr.Kill(Core::ProcessMgr.CurrentProc);
        }

        void YIELD(SystemCallArguments args)
        {
            Core::ProcessMgr.Schedule();
        }

        void CMD(SystemCallArguments args)
        {
            char* command = (char*)args.Source;
            Core::CLI.Execute(command);
        }
    }
}
