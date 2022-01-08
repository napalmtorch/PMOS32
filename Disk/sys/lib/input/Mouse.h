#pragma once
#include <sys/lib/stdint.h>
#include <sys/lib/stdarg.h>
#include <sys/lib/stddef.h>

namespace pmlib
{
    uint32_t GetMouseX();
    uint32_t GetMouseY();
    bool     GetMouseLeftBtn();
    bool     GetMouseRightBtn();
}