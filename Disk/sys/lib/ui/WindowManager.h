#pragma once
#include <sys/lib/ui/Window.h>

namespace pmgui
{
    class WindowManager
    {
        friend class Window;

        public:
            void Init();
    };
}