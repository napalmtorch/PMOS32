#pragma once
#include <sys/lib/stdint.h>
#include <sys/lib/stdarg.h>
#include <sys/lib/stddef.h>
#include <sys/lib/ui/Container.h>
#include <sys/lib/ui/Button.h>

namespace pmgui
{
    class Window;
    class WindowManager;

    class Window : public Container
    {
        friend class WindowManager;

        protected:
            pmlib::Rectangle TitleBarBounds;
            Button CloseBtn, MaxBtn, MinBtn;

        private:
            int RefreshTime, RefreshTimeLast, RefreshTick;
            int MoveStartX, MoveStartY;
            bool MoveClick, Moving;

        public:
            Window();
            Window(int x, int y, int w, int h, char* name, char* title);
            void Dispose() override;
            void Update() override;
            void Draw() override;
            void DrawTitleBar();
            void Render() override;

            void OnClick() override;
            void OnMouseDown() override;
            void OnMouseUp() override;
            void OnMouseHover() override;
            void OnMouseEnter() override;
            void OnMouseLeave() override;
    };
}