#pragma once
#include <sys/lib/stdint.h>
#include <sys/lib/stdarg.h>
#include <sys/lib/stddef.h>
#include <sys/lib/string.h>
#include <sys/lib/api/Memory.h>
#include <sys/lib/gfx/Image.h>

namespace pmlib
{
    extern Image ScreenBuffer;

    void Render(int x, int y, int w, int h, uint32_t* data);
    uint32_t* GetFrameBuffer();
    uint32_t  GetFrameBufferWidth();
    uint32_t  GetFrameBufferHeight();
}