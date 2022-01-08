#include <Lib/Memory.h>

EXTC
{
    void* memchr(const void* str, int c, size_t n)
    {
        uint8_t* s = (uint8_t*)str;
        for (uint64_t i = 0; i < n; i++)
        {
            if (s[i] == (uint8_t)c) { return (void*)(str + i); }
        }
        return nullptr;
    }

    int memcmp(const void* str1, const void* str2, size_t n)
    {
        uint8_t* p = (uint8_t*)str1;
        uint8_t* q = (uint8_t*)str2;
        int state = 0;

        if (str1 == str2) { return state; }
        while (n > 0)
        {
            if (*p != *q) { state = (*p > *q) ? 1 : -1; break; }
            n--;
            p++;
            q++;
        }
        return state;
    }

    int memicmp(const void* str1, const void* str2, size_t n) { return 0; }

    void* memcpy(void* dest, const void* src, size_t n)
    {
        uint32_t  num_dwords = n / 4;
        uint32_t  num_bytes  = n % 4;
        uint8_t*  dest8      = ((uint8_t*)dest) + num_dwords * 4;
        uint8_t*  src8       = ((uint8_t*)src) + num_dwords * 4;
        register uint32_t i;

        for (i = 0; i < num_dwords; i++) { ((uint32_t*)dest)[i] = ((uint32_t*)src)[i]; }
        for (i = 0; i < num_bytes; i++)  { dest8[i]  = src8[i]; }
        return dest;
    }

    void* memmove(void* dest, const void* src, size_t n) { return memcpy(dest, src, n); }

    void* memset(void* dest, int c, size_t n)
    {
        uint32_t len = n / 4;
        asm volatile ("cld; rep stosl" : "+c" (len), "+D" (dest) : "a" (c) : "memory");
	    return dest;
    }
}