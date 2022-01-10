#include <sys/lib/stdint.h>
#include <sys/lib/stdarg.h>
#include <sys/lib/stddef.h>
#include <sys/lib/stdlib.h>
#include <sys/lib/stdio.h>
#include <sys/lib/api/Library.h>
#include <sys/lib/api/Time.h>
#include <sys/lib/gfx/Graphics.h>
#include <sys/lib/gfx/Image.h>
#include <sys/lib/input/Keyboard.h>
#include <sys/lib/input/Mouse.h>
#include <sys/lib/ui/Window.h>

int _start(void* arg)
{
    lib_init();
    printf("Test program\n");

    pmgui::Window* win = new pmgui::Window(128, 128, 320, 240, "Terminal", "Terminal");
    ProcessMessage msg = { "CREATEWIN", sizeof(pmgui::Window), (uint8_t*)win };
    pmlib::SendProcMsg("Shell.elf", msg);
    yield();

    while (true)
    {
        yield();
    }

    win->Dispose();
    kfree(win);
    return 0;
}