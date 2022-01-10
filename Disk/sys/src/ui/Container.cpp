#include <sys/lib/ui/Container.h>
#include <sys/lib/gfx/Graphics.h>
#include <sys/lib/stdio.h>
using namespace pmlib;

namespace pmgui
{
    Container::Container(int x, int y, int w, int h, char* name) : Control(x, y, w, h, name, nullptr)
    {
        Type = ControlType::Container;
        Buffer.Create(w, h);
        Draw();
    }
    
    void Container::Dispose()
    {
        Control::Dispose();
        if (Buffer.GetData() != nullptr) { kfree(Buffer.GetData()); }
    }

    void Container::Update()
    {
        Control::Update();
    }

    void Container::Draw()
    {
        Control::Draw();
        Buffer.DrawFilledRect(0, 0, Bounds.Width, Bounds.Height, Style.Colors[0]);
        
        if (Style.Border == BorderStyle::Classic) { Buffer.DrawRect3D(0, 0, Bounds.Width, Bounds.Height, false, Style.Colors[2], Style.Colors[3], Style.Colors[4]); }
        else if (Style.Border == BorderStyle::Flat) { Buffer.DrawRect(0, 0, Bounds.Width, Bounds.Height, Style.BorderSize, Style.Colors[2]); }
    }

    void Container::Render()
    {
        ScreenBuffer.DrawArray(Bounds.X, Bounds.Y, Bounds.Width, Bounds.Height, Buffer.GetData());
    }

    void Container::OnClick()
    {
        Control::OnClick();
        Draw();
    }

    void Container::OnMouseDown()
    {
        Control::OnMouseDown();
        if (!State.Down) { Draw(); }
    }

    void Container::OnMouseUp()
    {
        Control::OnMouseUp();
        if (State.Clicked) { Draw(); }
    }

    void Container::OnMouseHover()
    {
        Control::OnMouseHover();
    }

    void Container::OnMouseEnter()
    {
        Control::OnMouseEnter();
        Draw();
    }

    void Container::OnMouseLeave()
    {
        Control::OnMouseLeave();
        Draw();
    }
}