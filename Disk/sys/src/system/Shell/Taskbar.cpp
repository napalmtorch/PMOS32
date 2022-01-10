#include <sys/lib/system/Shell/Taskbar.h>
#include <sys/lib/system/Shell/Shell.h>
#include <sys/lib/ui/Style.h>

void ShellTaskbar::Init()
{
    printf("Initialized taskbar\n");
    Bounds = pmlib::Rectangle(0, pmlib::GetFrameBufferHeight() - 22, pmlib::GetFrameBufferWidth(), 22);
}

void ShellTaskbar::Update()
{

}

void ShellTaskbar::Draw()
{
    pmlib::ScreenBuffer.DrawFilledRect(Bounds.X, Bounds.Y, Bounds.Width, Bounds.Height, pmgui::Styles::Default.Colors[0]);
    pmlib::ScreenBuffer.DrawRect3D(Bounds.X, Bounds.Y, Bounds.Width, Bounds.Height, false, 
                                    pmgui::Styles::Default.Colors[2], pmgui::Styles::Default.Colors[3], pmgui::Styles::Default.Colors[4]);

    pmlib::DateTime time = pmlib::GetCurrentTime();
    memset(TimeString, 0, 64);
    pmlib::GetTimeString(time, false, true, TimeString);
    pmlib::ScreenBuffer.DrawString(Bounds.X + Bounds.Width - (strlen(TimeString) * 8) - 8, Bounds.Y + 7, TimeString, pmgui::Styles::Default.Colors[1], pmlib::Fonts::Serif8x8);
}