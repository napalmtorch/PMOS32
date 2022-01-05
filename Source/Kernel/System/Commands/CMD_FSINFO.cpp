#include <Kernel/System/CLI.h>
#include <Kernel/Core/Kernel.h>

namespace System
{
    namespace CommandMethods
    {
        void FSINFO(char* input, char** argv, int argc)
        {
            if (argc == 2)
            {
                if (streql(argv[1], "-b")) { Core::FileSysHDD.BlockEntries.Print(DebugMode::Terminal); }
                else if (streql(argv[1], "-f")) { Core::FileSysHDD.FileEntries.Print(DebugMode::Terminal); }
            }
        }
    }
}