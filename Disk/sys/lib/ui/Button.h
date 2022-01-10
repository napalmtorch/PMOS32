#pragma once
#include <sys/lib/stdint.h>
#include <sys/lib/stdarg.h>
#include <sys/lib/stddef.h>
#include <sys/lib/ctype.h>
#include <sys/lib/api/Util.h>
#include <sys/lib/ui/Style.h>
#include <sys/lib/ui/Control.h>
#include <sys/lib/gfx/Image.h>

namespace pmgui
{
    class Button : public Control
    {
        public:
            Button(Container* parent = nullptr);
            Button(int x, int y, char* name, Container* parent);
            void Update() override;
            void Draw() override;

            void OnClick() override;
            void OnMouseDown() override;
            void OnMouseUp() override;
            void OnMouseHover() override;
            void OnMouseEnter() override;
            void OnMouseLeave() override;
    };
}