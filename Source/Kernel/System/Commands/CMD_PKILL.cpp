#include <Kernel/System/CLI.h>
#include <Kernel/Core/Kernel.h>

namespace System
{
    namespace CommandMethods
    {
        void PKILL(char* input, char** argv, int argc)
        {
            if (argc != 3) { Debug::Error("Invalid arguments", DebugMode::Terminal); return; }

            if (streql(argv[1], "-i"))
            {
                uint32_t i = atol(argv[2]);
                if (i == 0) { Debug::Error("Cannot terminate kernel process", DebugMode::All); return; }
                if (Core::ProcessMgr.Kill(i)) { Debug::OK("Killed process at index %d", DebugMode::All, i); }
                else { Debug::Error("Unable to locate process at index %d", DebugMode::All, i); }
            }
            else { Debug::Error("Invalid arguments", DebugMode::Terminal); }
        }
    }
}