#include <Kernel/System/CLI.h>
#include <Kernel/Core/Kernel.h>
using namespace HAL;
using namespace Graphics;

namespace System
{
    void CommandLine::Init()
    {
        Count    = 0;
        CountMax = 256;
        Commands = (Command**)Core::Heap.Allocate(sizeof(Command*) * CountMax, true, Memory::HeapType::PointerArray);

        ArgumentCount = 0;
        Arguments     = nullptr;
        EnterDown     = false;

        Register((Command*)&Commands::CLS);
        Register((Command*)&Commands::HELP);
        Register((Command*)&Commands::HEAP);
        Register((Command*)&Commands::PMM);
        Register((Command*)&Commands::VMM);
        Register((Command*)&Commands::PROC);
        Register((Command*)&Commands::FSINFO);
        Register((Command*)&Commands::EXEC);
        Register((Command*)&Commands::PKILL);

        Register((Command*)&Commands::LS);
        Register((Command*)&Commands::CD);

        char* kb_data = (char*)Core::Heap.Allocate(512, true, Memory::HeapType::String);
        KBStream = Stream((uint8_t*)kb_data, 512);
        Enable();

        CurrentPath = (char*)Core::Heap.Allocate(128, true, Memory::HeapType::String);
        strcpy(CurrentPath, "/");

        Debug::OK("Initialized command line");
    }

    void CommandLine::Enable()
    {
        Core::Keyboard.SetStream(&KBStream);
        Core::Keyboard.TerminalOutput = true;
        Enabled = true;
    }

    void CommandLine::Disable()
    {
        if (Core::Keyboard.GetStream() == &KBStream) { Core::Keyboard.SetStream(nullptr); }
        Core::Keyboard.TerminalOutput = false;
        Core::Terminal.Clear(Graphics::Color4::Black);
        Core::VESA.Clear(Graphics::Color::Black);
        Enabled = false;
    }

    void CommandLine::PrintCaret()
    {
        if (!Enabled) { return; }
        Core::Terminal.Write("root", Color4::Magenta);
        Core::Terminal.Write("@");
        Core::Terminal.Write(CurrentPath, Color4::Yellow);
        Core::Terminal.Write("> ", Color4::DarkGray);
    }

    bool CommandLine::Register(Command* cmd)
    {
        if (cmd == nullptr) { Debug::Error("Tried to register null command"); return false; }
        int i = GetFreeIndex();
        if (i < 0 || i >= CountMax) { Debug::Error("Maximum amount of registered commands reached"); return false; }
        Commands[i] = cmd;
        Count++;
        Debug::Info("Registered command - INDEX: %d, NAME: '%s'", i, cmd->Name);
        return true;
    }

    bool CommandLine::Unregister(Command* cmd)
    {
        if (cmd == nullptr) { Debug::Error("Tried to un-register null command"); return false; }
        for (uint32_t i = 0; i < CountMax; i++)
        {
            if (Commands[i] == cmd)
            {
                Debug::Info("Un-registered command - INDEX: %d, NAME: '%s'", i, Commands[i]->Name);
                Commands[i] = nullptr;
                return true;
            }
        }
        
        Debug::Error("Unable to un-register command '%s'", cmd->Name);
        return false;
    }

    void CommandLine::Execute(char* input)
    {
        Arguments = strsplit(input, ' ', &ArgumentCount);

        if (strlen(input) == 0 || input == nullptr) { return; }

        if (ArgumentCount == 0)
        {
            Core::Heap.FreeArray((void**)Arguments, ArgumentCount);
            return;
        }

        char* cmd = (char*)Core::Heap.Allocate(strlen(input), true, Memory::HeapType::String);
        strcpy(cmd, Arguments[0]);
        strupr(cmd);

        for (uint32_t i = 0; i < CountMax; i++)
        {
            if (Commands[i] == nullptr) { continue; }
            if (streql(Commands[i]->Name, cmd))
            {
                if (Commands[i]->Callback != nullptr) { Commands[i]->Callback(input, Arguments, ArgumentCount); }
                else { Debug::Error("Invalid callback for command '%s'", DebugMode::Terminal, Commands[i]->Name); return; }
                Core::Heap.Free(cmd);
                Core::Heap.FreeArray((void**)Arguments, ArgumentCount);
                return;
            }
        }

        Debug::Error("Unknown command '%s'", DebugMode::Terminal, Arguments[0]);
        Core::Heap.Free(cmd);
        Core::Heap.FreeArray((void**)Arguments, ArgumentCount);
    }

    void CommandLine::Monitor()
    {
        if (!Enabled) { EnterDown = false; return; }
        if (Core::Keyboard.IsKeyDown(Key::Enter) && !EnterDown)
        {
            Execute((char*)Core::Keyboard.GetStream()->Data);
            memset(Core::Keyboard.GetStream()->Data, 0, Core::Keyboard.GetStream()->Size);
            Core::Keyboard.GetStream()->Seek(0);
            PrintCaret();
            EnterDown = true;
        }

        if (Core::Keyboard.IsKeyUp(Key::Enter)) { EnterDown = false; }
    }

    char* CommandLine::GetPath(char* path)
    {
        char* output = (char*)Core::Heap.Allocate(strlen(path) + strlen(CurrentPath) + 1, true, Memory::HeapType::String);
        if (path[0] != '/') 
        { 
            strcpy(output, CurrentPath); 
            
            if (output[strlen(output) - 1] != '/') { stradd(output, '/'); }
            if (path != nullptr && strlen(path) > 0) { strcat(output, path); }
        }
        else { strcpy(output, path); }
        return output;
    }

    int CommandLine::GetFreeIndex()
    {
        for (uint32_t i = 0; i < CountMax; i++)
        {
            if (Commands[i] == nullptr) { return i; }
        }
        return -1;
    }
}