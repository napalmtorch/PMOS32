#include <Kernel/System/CLI.h>
#include <Kernel/Core/Kernel.h>
using namespace HAL;

namespace System
{
    namespace CommandMethods
    {
        void LS(char* input, char** argv, int argc)
        {
            if (argc == 1)
            {
                char* path = Core::CLI.CurrentPath;

                uint32_t dirs_count = 0;
                char** dirs = Core::FS.GetDirectories(path, &dirs_count);
                for (int i = 0; i < dirs_count; i++)
                {
                    Core::Terminal.Write("- ");
                    Core::Terminal.WriteLine(dirs[i], Graphics::Color4::Yellow);
                }
                Core::Heap.FreeArray((void**)dirs, dirs_count);

                uint32_t files_count = 0;
                char** files = Core::FS.GetFiles(path, &files_count);
                for (int i = 0; i < files_count; i++) { Core::Terminal.WriteLine(files[i]); }
                Core::Heap.FreeArray((void**)files, files_count);
            }
            else if (argc == 2)
            {
                char* path = (char*)((uint32_t)input + 3);
                char* new_path = Core::CLI.GetPath(path);
                path = new_path;

                uint32_t dirs_count = 0;
                char** dirs = Core::FS.GetDirectories(path, &dirs_count);
                for (int i = 0; i < dirs_count; i++)
                {
                    Core::Terminal.Write("- ");
                    Core::Terminal.WriteLine(dirs[i], Graphics::Color4::Yellow);
                }
                Core::Heap.FreeArray((void**)dirs, dirs_count);
                if (dirs_count == 0) { Core::Terminal.WriteLine("No directories found", Graphics::Color4::DarkGray); }

                uint32_t files_count = 0;
                char** files = Core::FS.GetFiles(path, &files_count);
                for (int i = 0; i < files_count; i++) 
                { 
                    char* full_path = (char*)Core::Heap.Allocate(strlen(new_path) + strlen(files[i]) + 2, true, Memory::HeapType::String);
                    strcpy(full_path, new_path);
                    stradd(full_path, '/');
                    strcat(full_path, files[i]);
                    FileSystem::File file = Core::FS.GetFileInfo(full_path);
                    Core::Heap.Free(full_path);
                    Core::Terminal.Write(files[i]); 
                    if (Core::Terminal.Type == TerminalType::VESA) { Core::VESA.DrawFilledRect(Core::Terminal.GetCursorX() * 8, Core::Terminal.GetCursorY() * 16, 8, 16, ConvertColor4ToColor(Core::Terminal.GetBackColor())); }
                    Core::Terminal.SetCursorX(24);
                    char temp[32];
                    memset(temp, 0, 32);
                    ltoa(file.Size, temp, 10);
                    Core::Terminal.Write(temp, Graphics::Color4::Cyan);
                    Core::Terminal.WriteLine(" BYTES", Graphics::Color4::Cyan);
                    Core::Heap.Free(file.Name);
                    Core::Heap.Free(file.Path);
                }
                Core::Heap.FreeArray((void**)files, files_count);
                Core::Heap.Free(new_path);
                if (files_count == 0) { Core::Terminal.WriteLine("No files found", Graphics::Color4::DarkGray); }
            }
            else { Debug::Error("Invalid arguments", DebugMode::Terminal); }
        }
    }
}