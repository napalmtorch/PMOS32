#include <Kernel/System/CLI.h>
#include <Kernel/Core/Kernel.h>

namespace System
{
    namespace CommandMethods
    {
        void CLS(char* input, char** argv, int argc)
        {
            Core::Terminal.Clear();
        }
    }
}