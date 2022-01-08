#pragma once
#include <sys/lib/stdint.h>
#include <sys/lib/stdarg.h>
#include <sys/lib/stddef.h>

namespace pmgui
{
    class Window;
    class WindowManager;

    class Window
    {
        friend class WindowManager;

        private:
            char     Name[32];
            uint32_t ID;
            int      X, Y, Width, Height;

        public:
            Window(int x, int y, int w, int h, char* name);
            virtual void Update();
            virtual void Draw();
    };
}