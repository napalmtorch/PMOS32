#include <Kernel/System/CLI.h>
#include <Kernel/Core/Kernel.h>

namespace System
{
    namespace CommandMethods
    {
        void CD(char* input, char** argv, int argc)
        {
            if (argc == 1) { return; }

            if (argc >= 2)
            {
                char* path = (char*)((uint32_t)input + 3);
                char* new_path = Core::CLI.GetPath(path);
                if (!Core::FS.DirectoryExists(new_path)) { Core::Heap.Free(new_path); Debug::Error("Unable to locate directory '%s'", DebugMode::Terminal, path); return; }
                Core::Heap.Free(Core::CLI.CurrentPath);
                Core::CLI.CurrentPath = new_path;
            }
        }
    }
}