#include <sys/lib/stdio.h>
#include <sys/lib/ui/Window.h>
#include <sys/lib/api/Library.h>
#include <sys/lib/input/Mouse.h>
#include <sys/lib/gfx/Graphics.h>

namespace pmgui
{
    Window::Window() : Container(0, 0, 0, 0, "")
    {
        Type = ControlType::Window;
        Style = Styles::DefaultWindow;
        TitleBarBounds = pmlib::Rectangle(1, 1, Bounds.Width - 3, 17);
    }

    Window::Window(int x, int y, int w, int h, char* name, char* title) : Container(x, y, w, h, name)
    {
        Type = ControlType::Window;
        Style = Styles::DefaultWindow;
        TitleBarBounds = pmlib::Rectangle(x + 1, y + 1, Bounds.Width - 3, 17);

        //if (Text != nullptr) { kfree(Text); }
        //Text = (char*)kmalloc(strlen(title), true, MEMTYPE_STRING);
        //strcpy(Text, title);

        CloseBtn = Button(0, 0, "", this);
        MaxBtn = Button(0, 0, "", this);
        MinBtn = Button(0, 0, "", this);

        Draw();
    }

    void Window::Dispose()
    {
        Container::Dispose();
        CloseBtn.Dispose();
        MaxBtn.Dispose();
        MinBtn.Dispose();
    }

    void Window::Update()
    {
        if (Type != ControlType::Window) { return; }
        Container::Update();

        if (Moving) { State.Focused = true; }
        if (State.Focused)
        {
            CloseBtn.Update();
            MaxBtn.Update();
            MinBtn.Update();

            // title bar hover
            if (TitleBarBounds.Intersects(pmlib::GetMouseX(), pmlib::GetMouseY()))
            {
                // title bar mouse down
                if (pmlib::GetMouseLeftBtn())
                {
                    if (!MoveClick)
                    {
                        MoveStartX = pmlib::GetMouseX() - Bounds.X;
                        MoveStartY = pmlib::GetMouseY() - Bounds.Y;
                        MoveClick = true;
                    }
                    Moving = true;

                    // redraw
                    RefreshTime = pmlib::GetCurrentTime().Millisecond;
                    if (RefreshTime != RefreshTimeLast) { RefreshTick++; RefreshTimeLast = RefreshTime; }

                    if (RefreshTick >= 16) { Draw(); RefreshTick = 0; }
                }
            }

            if (!pmlib::GetMouseLeftBtn() && MoveClick) { Moving = false; MoveClick = false; Draw(); }

            // move window
            if (Moving)
            {
                Bounds.X = pmlib::GetMouseX() - MoveStartX;
                Bounds.Y = pmlib::GetMouseY() - MoveStartY;
                if (Bounds.Y < 0) { Bounds.Y = 0; }
                if (Bounds.Y >= (int)pmlib::GetFrameBufferHeight() - 24) { Bounds.Y = pmlib::GetFrameBufferHeight() - 24; }

                DrawTitleBar();
            }
        }
    }

    void Window::Draw()
    {
        if (Type != ControlType::Window) { return; }
        Container::Draw();
        DrawTitleBar();
    }

    void Window::DrawTitleBar()
    {
        TitleBarBounds.X = Bounds.X + 1;
        TitleBarBounds.Y = Bounds.Y + 1;
        TitleBarBounds.Width = Bounds.Width - 3;

        CloseBtn.Bounds = pmlib::Rectangle((TitleBarBounds.X + TitleBarBounds.Width - 15) - Bounds.X, (TitleBarBounds.Y + 2) - Bounds.Y, 14, 13);
        MaxBtn.Bounds = pmlib::Rectangle(CloseBtn.Bounds.X - 16, CloseBtn.Bounds.Y, 14, 13);
        MinBtn.Bounds = pmlib::Rectangle(MaxBtn.Bounds.X - 16, MaxBtn.Bounds.Y, 14, 13);

        Buffer.DrawFilledRect(TitleBarBounds.X - Bounds.X, TitleBarBounds.Y - Bounds.Y, TitleBarBounds.Width, TitleBarBounds.Height, (uint32_t)pmlib::Color::DarkBlue);
        if (Text != nullptr) { Buffer.DrawString(4, 5, Text, Style.Colors[6], pmlib::Fonts::Serif8x8); }

        CloseBtn.Draw();
        MaxBtn.Draw();
        MinBtn.Draw();
    }
    
    void Window::Render()
    {
        Container::Render();
    }

    void Window::OnClick()
    {
        Container::OnClick();
    }

    void Window::OnMouseDown()
    {
        Container::OnMouseDown();
    }

    void Window::OnMouseUp()
    {
        Container::OnMouseUp();
    }

    void Window::OnMouseHover()
    {
        Container::OnMouseHover();
    }

    void Window::OnMouseEnter()
    {
        Container::OnMouseEnter();
    }

    void Window::OnMouseLeave()
    {
        Container::OnMouseLeave();
    }
}