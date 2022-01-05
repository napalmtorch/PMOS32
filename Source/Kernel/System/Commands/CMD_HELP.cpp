#include <Kernel/System/CLI.h>
#include <Kernel/Core/Kernel.h>
using namespace HAL;
using namespace Graphics;

namespace System
{
    namespace CommandMethods
    {
        void HELP(char* input, char** argv, int argc)
        {
            for (uint32_t i = 0; i < Core::CLI.CountMax; i++)
            {
                if (Core::CLI.Commands[i] == nullptr) { continue; }
                Core::Terminal.Write("- ", Color4::DarkGray);
                Core::Terminal.Write(Core::CLI.Commands[i]->Name, Color4::Yellow);
                if (Core::Terminal.Type == TerminalType::VESA) { Core::VESA.DrawFilledRect(Core::Terminal.GetCursorX() * 8, Core::Terminal.GetCursorY() * 16, 8, 16, ConvertColor4ToColor(Core::Terminal.GetBackColor())); }
                Core::Terminal.SetCursorX(24);
                Core::Terminal.WriteLine(Core::CLI.Commands[i]->Help);
            }
        }
    }
}