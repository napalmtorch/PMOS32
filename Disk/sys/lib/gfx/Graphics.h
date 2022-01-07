#pragma once
#include <sys/lib/Types.h>
#include <sys/lib/Memory.h>

namespace pmlib
{
    void Render(int x, int y, int w, int h, uint32_t* data);
    uint32_t* GetFrameBuffer();
    uint32_t  GetFrameBufferWidth();
    uint32_t  GetFrameBufferHeight();
}