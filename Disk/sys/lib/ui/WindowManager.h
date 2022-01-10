#pragma once
#include <sys/lib/ui/Window.h>

namespace pmgui
{
    class WindowManager
    {
        friend class Window;

        public:
            Window** Windows;
            Window*  ActiveWindow;
            uint32_t Count;
            uint32_t CountMax;
            int ActiveIndex;
            bool MouseDown;

        public:
            void Init();
            void Dispose();
            void Update();
            void Render();

        public:
            bool Add(Window* window);
            bool Remove(Window* window, bool free = true);

        public:
            Window* SetActiveWindow(Window* win);
    };
}