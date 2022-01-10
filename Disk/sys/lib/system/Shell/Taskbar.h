#pragma once
#include <sys/lib/stdint.h>
#include <sys/lib/stdarg.h>
#include <sys/lib/stddef.h>
#include <sys/lib/stdlib.h>
#include <sys/lib/stdio.h>
#include <sys/lib/string.h>
#include <sys/lib/api/Util.h>

class ShellTaskbar
{
    public:
        pmlib::Rectangle Bounds;

    private:
        char TimeString[64];

    public:
        void Init();
        void Update();
        void Draw();
};