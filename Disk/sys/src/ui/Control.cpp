#include <sys/lib/ui/Control.h>
#include <sys/lib/ui/Container.h>
#include <sys/lib/stdlib.h>
#include <sys/lib/stdio.h>
#include <sys/lib/string.h>
#include <sys/lib/api/Library.h>
#include <sys/lib/api/Memory.h>
#include <sys/lib/input/Keyboard.h>
#include <sys/lib/input/Mouse.h>

using namespace pmlib;

namespace pmgui
{
    Control::Control(int x, int y, int w, int h, char* name, Container* parent)
    {
        this->Init(x, y, w, h, name, parent);
    }

    void Control::Init(int x, int y, int w, int h, char* name, Container* parent)
    {
        Parent = parent;
        Bounds = Rectangle(x, y, w, h);
        Style  = Styles::Default;
        Enabled = true;
        Visible = true;

        State.Hover     = false;
        State.Down      = false;
        State.Clicked   = false;
        State.Unclicked = false;
        State.Focused   = true;

        Tag = nullptr;

        if (name != nullptr)
        {
            if (strlen(name) > 0)
            {
                Name = (char*)kmalloc(strlen(name), true, MEMTYPE_STRING);
                Text = (char*)kmalloc(strlen(name), true, MEMTYPE_STRING);
                strcpy(Name, name);
                strcpy(Text, name);
            }
            else { Name = nullptr; Text = nullptr; }
        }
        else { Name = nullptr; Text = nullptr; }
    }

    void Control::Dispose()
    {
        kfree(Name);
        kfree(Text);
        kfree(Tag);
    }

    void Control::Update()
    {
        if (State.Focused)
        {
            int mx = GetMouseX();
            int my = GetMouseY();

            if (Parent == nullptr)
            {
                if (Bounds.Intersects(mx, my))
                {
                    if (!State.Hover) { OnMouseEnter(); }
                    State.Hover = true;
                    OnMouseHover();              

                    if (GetMouseLeftBtn() && !State.Clicked)
                    {
                        OnMouseDown();
                        State.Down = true;
                        State.Clicked = true;
                        State.Unclicked = false;
                    }
                    if (!GetMouseLeftBtn() && !State.Unclicked)
                    {
                        State.Down = false;
                        if (State.Clicked)
                        {
                            OnClick();
                            OnMouseUp();
                            State.Unclicked = true;
                            State.Clicked = false;
                        }
                    }
                }
                else 
                { 
                    if (State.Hover) { OnMouseLeave(); }
                    State.Down = false; State.Hover = false; State.Clicked = false; State.Unclicked = false; 
                }
            }
            else
            {
                pmlib::Rectangle bounds = pmlib::Rectangle(Bounds.X + Parent->Bounds.X, Bounds.Y + Parent->Bounds.Y, Bounds.Width, Bounds.Height);
                if (bounds.Intersects(mx, my))
                {
                    if (!State.Hover) { OnMouseEnter(); }
                    State.Hover = true;
                    OnMouseHover();

                    if (GetMouseLeftBtn() && !State.Clicked)
                    {
                        OnMouseDown();
                        State.Down = true;
                        State.Clicked = true;
                        State.Unclicked = false;
                    }
                    if (!GetMouseLeftBtn() && !State.Unclicked)
                    {
                        State.Down = false;
                        if (State.Clicked)
                        {
                            OnClick();
                            OnMouseUp();
                            State.Unclicked = true;
                            State.Clicked = false;
                        }
                    }
                }
                else 
                { 
                    if (State.Hover) { OnMouseLeave(); }
                    State.Down = false; State.Hover = false; State.Clicked = false; State.Unclicked = false; 
                }
            }
        }
    }

    void Control::Draw()
    {
        
    }

    void Control::OnClick()
    {
        //printf("MOUSE CLICK - %s", Name);
    }

    void Control::OnMouseDown()
    {

    }

    void Control::OnMouseUp()
    {

    }

    void Control::OnMouseHover()
    {

    }

    void Control::OnMouseEnter()
    {
        //printf("MOUSE ENTER - %s\n", Name);
    }

    void Control::OnMouseLeave()
    {
        //printf("MOUSE LEAVE - %s\n", Name);
    }
}