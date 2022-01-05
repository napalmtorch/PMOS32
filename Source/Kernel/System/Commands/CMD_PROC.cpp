#include <Kernel/System/CLI.h>
#include <Kernel/Core/Kernel.h>

namespace System
{
    namespace CommandMethods
    {
        void PROC(char* input, char** argv, int argc)
        {
            DebugMode old = Debug::GetMode();
            Debug::SetMode(DebugMode::Terminal);
            Core::ProcessMgr.Print();
            Debug::SetMode(old);
        }
    }
}