#include <sys/lib/ui/Control.h>
#include <sys/lib/stdlib.h>
#include <sys/lib/stdio.h>
#include <sys/lib/string.h>
#include <sys/lib/api/Library.h>
#include <sys/lib/api/Memory.h>
using namespace pmlib;

namespace pmgui
{
    Control::Control(int x, int y, int w, int h, char* name, void* parent)
    {
        this->Init(x, y, w, h, name, parent);
    }

    void Control::Init(int x, int y, int w, int h, char* name, void* parent)
    {
        Parent = parent;
        Bounds = Rectangle(x, y, w, h);
        Style  = Styles::Default;
        Enabled = true;
        Visible = true;

        Tag  = nullptr;
        Name = (char*)kmalloc(strlen(name), true, MEMTYPE_STRING);
        Text = (char*)kmalloc(strlen(name), true, MEMTYPE_STRING);
        strcpy(Name, name);
        strcpy(Text, name);
    }

    void Control::Update()
    {
        
    }

    void Control::Draw()
    {
        
    }

    void Control::OnClick()
    {

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

    void Control::OnMouseLeave()
    {

    }
}