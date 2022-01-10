#include <sys/lib/system/Shell/Shell.h>

void ShellHost::Init()
{
    // reset properties
    memset(&Time, 0, sizeof(ShellTime));
    ExitCode = 0;

    // disable command line
    clistate(false);

    // initialize standard library
    lib_init();

    // get screen dimensions
    Width  = pmlib::GetFrameBufferWidth();
    Height = pmlib::GetFrameBufferHeight();

    // initialize screen buffer
    pmlib::ScreenBuffer = pmlib::Image();
    pmlib::ScreenBuffer.Create(Width, Height);

    // initialize taskbar
    Taskbar.Init();

    // initialized window manager
    WinMgr = (pmgui::WindowManager*)kmalloc(sizeof(pmgui::WindowManager), true, MEMTYPE_OBJECT);
    WinMgr->Init();

    // finished
    printf("Finished starting shell\n");   
}

void ShellHost::Dispose()
{   
    kfree(WinMgr);
    kfree(pmlib::ScreenBuffer.GetData());
    clistate(true);
    system("cls");
}

void ShellHost::Update()
{
    if (pmlib::KeyboardDown(pmlib::Key::End)) { ExitCode = 1; }

    Taskbar.Update();

    Time.Ticks++;
    Time.Now = pmlib::GetCurrentTime();

    Time.CurrentSec = Time.Now.Second;
    if (Time.CurrentSec != Time.LastSec)
    {
        Time.TPS     = Time.Ticks;
        Time.FPS     = Time.Frames;
        Time.Ticks   = 0;
        Time.Frames  = 0;
        Time.LastSec = Time.CurrentSec;
        
        ltoa(Time.FPS, Time.FPSString, 10);
        ltoa(Time.TPS, Time.TPSString, 10);
    }

    Time.CurrentMillis = Time.Now.Millisecond;
    if (Time.CurrentMillis != Time.LastMillis) { Time.Timer++; Time.LastMillis = Time.CurrentMillis; }

    if (Time.Timer >= 18)
    {
        Time.Frames++;
        Time.Timer = 0;
        Draw();
    }
}

void ShellHost::Draw()
{
    pmlib::ScreenBuffer.Clear(0xFF007F7F);
    pmlib::ScreenBuffer.DrawString(0, 0, (char*)"FPS: ", 0xFFFFFFFF, pmlib::Fonts::Serif8x16);
    pmlib::ScreenBuffer.DrawString(40, 0, Time.FPSString, 0xFFFFFFFF, pmlib::Fonts::Serif8x16);

    pmlib::ScreenBuffer.DrawString(0, 16, (char*)"TPS: ", 0xFFFFFFFF, pmlib::Fonts::Serif8x16);
    pmlib::ScreenBuffer.DrawString(40, 16, Time.TPSString, 0xFFFFFFFF, pmlib::Fonts::Serif8x16);

    if (WinMgr != nullptr) { WinMgr->Update(); WinMgr->Render(); }

    Taskbar.Draw();

    pmlib::ScreenBuffer.DrawArray(pmlib::GetMouseX(), pmlib::GetMouseY(), 12, 20, 0xFFFF00FF, (uint32_t*)pmlib::MouseCursor);
    pmlib::Render(0, 0, Width, Height, pmlib::ScreenBuffer.GetData());
}