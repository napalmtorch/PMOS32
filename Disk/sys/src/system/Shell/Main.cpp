#include <sys/lib/system/Shell/Main.h>
#include <sys/lib/system/Shell/Shell.h>

ShellHost shell;

int _start(void* arg)
{
    shell.Init();

    char* file = (char*)kmalloc(128, true, MEMTYPE_STRING);
    strcpy(file, "/sys/bin/Terminal.elf");
    asm volatile("int $0x80": :"a"(0), "b"((uint32_t)file), "c"(0xA0), "d"(0));
    yield();
    kfree(file);

    pmgui::Window* win = new pmgui::Window(128, 128, 320, 240, "TestWin", "TestWin");
    shell.WinMgr->Add(win);

    //system("exec /sys/bin/Terminal.elf");

    while (true)
    {
        shell.Update();

        if (pmlib::IsProcMsgReady())
        {
            ProcessMessage msg = pmlib::ReceiveProcMsg();
            
            if (!strcmp(msg.Message, "CREATEWIN"))
            {
                pmgui::Window* win = (pmgui::Window*)msg.Data;
                if (!shell.WinMgr->Add(win)) { printf("Unable to add window\n"); break; }
                printf("DONE ADDING WINDOW\n");
            }
        }

        if (shell.ExitCode > 0) { break; }
        yield();
    }

    shell.Dispose();

    yield();
    return shell.ExitCode;
}

void _draw_shell()
{
}