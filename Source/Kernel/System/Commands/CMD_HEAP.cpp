#include <Kernel/System/CLI.h>
#include <Kernel/Core/Kernel.h>
using namespace HAL;
using namespace Graphics;

namespace System
{
    namespace CommandMethods
    {
        void HEAP(char* input, char** argv, int argc)
        {
            DebugMode old = Debug::GetMode();
            Debug::SetMode(DebugMode::Terminal);
            Core::Heap.Print();
            Debug::SetMode(old);
        }
    }
}