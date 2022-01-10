#include <sys/lib/ui/Button.h>
#include <sys/lib/gfx/Graphics.h>
#include <sys/lib/stdio.h>
#include <sys/lib/ui/Container.h>
using namespace pmlib;

namespace pmgui
{
    Button::Button(Container* parent) : Control(0, 0, 92, 22, "", parent)
    {
        
    }

    Button::Button(int x, int y, char* name, Container* parent) : Control(x, y, 92, 22, name, parent)
    {
        Type = ControlType::Button;
    }

    void Button::Update()
    {
        Control::Update();
    }

    void Button::Draw()
    {
        Control::Draw();

        if (Parent != nullptr)
        {
            Parent->Buffer.DrawFilledRect(Bounds.X, Bounds.Y, Bounds.Width, Bounds.Height, Style.Colors[0]);
    
            if (Style.Border == BorderStyle::Classic) { Parent->Buffer.DrawRect3D(Bounds.X, Bounds.Y, Bounds.Width, Bounds.Height, State.Down, Style.Colors[2], Style.Colors[3], Style.Colors[4]); }
            else if (Style.Border == BorderStyle::Flat) { Parent->Buffer.DrawRect(Bounds.X, Bounds.Y, Bounds.Width, Bounds.Height, Style.BorderSize, Style.Colors[2]); }
        }
        else
        {
            ScreenBuffer.DrawFilledRect(Bounds.X, Bounds.Y, Bounds.Width, Bounds.Height, Style.Colors[0]);
    
            if (Style.Border == BorderStyle::Classic) { ScreenBuffer.DrawRect3D(Bounds.X, Bounds.Y, Bounds.Width, Bounds.Height, State.Down, Style.Colors[2], Style.Colors[3], Style.Colors[4]); }
            else if (Style.Border == BorderStyle::Flat) { ScreenBuffer.DrawRect(Bounds.X, Bounds.Y, Bounds.Width, Bounds.Height, Style.BorderSize, Style.Colors[2]); }
        }
    }

    void Button::OnClick()
    {
        Control::OnClick();
    }

    void Button::OnMouseDown()
    {
        Control::OnMouseDown();
    }

    void Button::OnMouseUp()
    {
        Control::OnMouseUp();
    }

    void Button::OnMouseHover()
    {
        Control::OnMouseHover();
    }

    void Button::OnMouseEnter()
    {
        Control::OnMouseEnter();
    }

    void Button::OnMouseLeave()
    {
        Control::OnMouseLeave();
    }
}