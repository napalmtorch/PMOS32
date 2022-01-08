#include <Kernel/System/SystemCall.h>
#include <Kernel/Core/Kernel.h>
using namespace HAL;
using namespace Graphics;

namespace System
{
    namespace SystemCalls
    {
        SystemCall SystemCalls[256];
        uint32_t   Count;

        void Init()
        {
            Count = 0;

            Register(SystemCalls::PRINT);
            Register(SystemCalls::MTABLE);
            Register(SystemCalls::EXEC);
            Register(SystemCalls::LOCK);
            Register(SystemCalls::UNLOCK);
            Register(SystemCalls::CLISTATE);
            Register(SystemCalls::EXIT);
            Register(SystemCalls::YIELD);

            IDT::RegisterInterrupt(IRQ_SYSCALL, Execute);

            Debug::Info("Initialized system calls");
        }

        void Register(SystemCall call)
        {
            if (Count >= 256) { Debug::Error("Maximum amount of system calls reached"); return; }
            SystemCalls[Count] = call;
            Debug::Info("Registered system call - CODE: 0x%8x, NAME: '%s'", call.Code, call.Name);
            Count++;
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
                    SystemCalls[i].Handler(args);
                    return;
                }
            }
            Debug::Error("Invalid system call 0x%8x", args.Code);
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
    }
}
