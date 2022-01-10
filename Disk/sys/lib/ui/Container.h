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
    class Container : public Control
    {
        public:
            pmlib::Rectangle ClientBounds;
            pmlib::Image Buffer;


        public:
            Container(int x, int y, int w, int h, char* name);
            void Dispose() override;
            void Update() override;
            void Draw() override;
            virtual void Render();

            void OnClick() override;
            void OnMouseDown() override;
            void OnMouseUp() override;
            void OnMouseHover() override;
            void OnMouseEnter() override;
            void OnMouseLeave() override;
    };
}