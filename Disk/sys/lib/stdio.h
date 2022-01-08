#pragma once
#include <sys/lib/stdarg.h>
#include <sys/lib/stdint.h>
#include <sys/lib/stddef.h>

EXTC
{
    int vprintf(const char* fmt, va_list args);
    int printf(const char* fmt, ...);
    int scanf(const char* fmt, ...);
}