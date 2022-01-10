#pragma once
#include <sys/lib/stdint.h>
#include <sys/lib/stdarg.h>
#include <sys/lib/stddef.h>
#include <sys/lib/stdlib.h>
#include <sys/lib/stdio.h>
#include <sys/lib/string.h>
#include <sys/lib/api/Library.h>
#include <sys/lib/api/Time.h>
#include <sys/lib/gfx/Graphics.h>
#include <sys/lib/gfx/Image.h>
#include <sys/lib/input/Keyboard.h>
#include <sys/lib/input/Mouse.h>
#include <sys/lib/ui/Control.h>
#include <sys/lib/ui/Window.h>
#include <sys/lib/ui/Container.h>
#include <sys/lib/ui/Button.h>
#include <sys/lib/ui/WindowManager.h>
#include <sys/lib/system/Shell/Taskbar.h>

typedef struct
{
    char FPSString[32];
    char TPSString[32];
    uint32_t FPS, TPS, Frames, Ticks;
    uint32_t CurrentSec, LastSec;
    uint32_t CurrentMillis, LastMillis;
    uint32_t Timer;
    pmlib::DateTime Now;
} ShellTime;

class ShellHost
{
    public:
        ShellTaskbar Taskbar;
        pmgui::WindowManager* WinMgr;
        uint32_t ExitCode;

    private:
        uint32_t Width, Height;
        ShellTime Time;

    public:
        void Init();
        void Dispose();
        void Update();
        void Draw();
};