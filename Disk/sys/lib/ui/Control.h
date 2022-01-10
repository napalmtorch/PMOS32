#pragma once
#include <sys/lib/stdint.h>
#include <sys/lib/stdarg.h>
#include <sys/lib/stddef.h>
#include <sys/lib/ctype.h>
#include <sys/lib/api/Util.h>
#include <sys/lib/ui/Style.h>

namespace pmgui
{
    enum class ControlType : uint8_t
    {
        Base,
        Container,
        Window,
        Button,
        TextBox,
        CheckBox,
    };

    typedef struct
    {
        bool Hover;
        bool Down;
        bool Clicked;
        bool Unclicked;
        bool Focused;
    } PACKED ControlInputState;

    class Container;

    class Control
    {
        protected:
            char*             Name;
            char*             Text;
            char*             Tag;

        public:
            Container*        Parent;
            bool              Enabled;
            bool              Visible;
            pmlib::Rectangle  Bounds;
            VisualStyle       Style;
            ControlType       Type;
            ControlInputState State;;

        public:
            Control(int x, int y, int w, int h, char* name, Container* parent = nullptr);
            void Init(int x, int y, int w, int h, char* name, Container* parent = nullptr);
            virtual void Dispose();
            virtual void Update();
            virtual void Draw();

        public:
            virtual void OnClick();
            virtual void OnMouseDown();
            virtual void OnMouseUp();
            virtual void OnMouseHover();
            virtual void OnMouseEnter();
            virtual void OnMouseLeave();

        public:
            void (*Click)(void* parent);
            void (*MouseDown)(void* parent);
            void (*MouseUp)(void* parent);
            void (*MouseHover)(void* parent);
            void (*MouseEnter)(void* parent);
            void (*MouseLeave)(void* parent);
    };
}