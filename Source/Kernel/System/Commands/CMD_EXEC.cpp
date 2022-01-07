#include <Kernel/System/CLI.h>
#include <Kernel/Core/Kernel.h>

namespace System
{
    namespace CommandMethods
    {
        void EXEC(char* input, char** argv, int argc)
        {
            if (argc == 1) { Debug::Error("Invalid arguments", DebugMode::Terminal); return; }

            SystemCallArguments args;
            args.Argument = 0;
            args.Source = (void*)argv[1];
            args.Destination = nullptr;
            args.Code = SystemCalls::EXEC.Code;
            SystemCalls::Execute(args);
        }
    }
}