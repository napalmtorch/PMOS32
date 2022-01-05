#pragma once
#include <Lib/Types.h>

EXTC
{
    void* memchr(const void* str, int c, size_t n);
    int   memcmp(const void* str1, const void* str2, size_t n);
    int   memicmp(const void* str1, const void* str2, size_t n);
    void* memcpy(void* dest, const void* src, size_t n);
    void* memmove(void* dest, const void* src, size_t n);
    void* memset(void* dest, int c, size_t n);
}