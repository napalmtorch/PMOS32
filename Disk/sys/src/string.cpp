#include <sys/lib/string.h>
#include <sys/lib/api/Library.h>
#include <sys/lib/api/Memory.h>

EXTC
{
    void* memchr(const void* str, int c, size_t n)
    {
        unsigned char *p = (unsigned char*)str;
        unsigned char *find = NULL;
        while((str != NULL) && (n--))
        {
            if( *p != (unsigned char)c) { p++; }
            else { find = p; break; }
        }
        return find;
    }

    int memcmp(const void* str1, const char* str2, size_t n)
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

    void* memmove(void* dest, const void* src, size_t n)
    {
        char *tmp  = (char *)kmalloc(n, true, MEMTYPE_USED);
        if (tmp == nullptr) { return nullptr; }
        uint32_t i = 0;
        for(i = 0; i < n; ++i) { *(tmp + i) = *(char*)((uint32_t)src + i); }
        for(i = 0; i < n; ++i) { *(char*)((uint32_t)dest + i) = *(tmp + i); }
        kfree(tmp);
        return dest;
    }

    void* memset(void* str, int c, size_t n)
    {
        n /= 4;
        asm volatile ("cld; rep stosl" : "+c" (n), "+D" (str) : "a" (c) : "memory");
        return str;
    }

    char* strcat(char* dest, const char* src)
    {
        if (dest == nullptr) { return nullptr; }
        if (src == nullptr) { return dest; }
        uint64_t src_len = strlen(src);
        uint64_t dest_len = strlen(dest);
        for (uint64_t i = 0; i < src_len; i++) { dest[dest_len + i] = src[i]; }
        dest[strlen(dest)] = 0;
        return dest;
    }

    char* strncat(char* dest, const char* src, size_t n)
    {
        if (dest == nullptr) { return nullptr; }
        if (src == nullptr) { return dest; }
        uint64_t dest_len = strlen(dest);
        for (uint64_t i = 0; i < n; i++) { dest[dest_len + i] = src[i]; }
        dest[strlen(dest)] = 0;
        return dest;
    }

    char* strchr(const char* str, int c)
    {
        return nullptr;
    }

    int strcmp(const char* str1, const char* str2)
    {
        while (*str1)
        {
            if (*str1 != *str2) { break; }
            str1++; str2++;
        }
        return *(char*)str1 - *(char*)str2;
    }

    int strncmp(const char* str1, const char* str2, size_t n)
    {
        while (n && *str1 && (*str1 == *str2))
        {
            ++str1;
            ++str2;
            --n;
        }
        if (n == 0) { return 0; }
        else { return (*(char*)str1 = *(char*)str2); }
        return 0;
    }

    char* strcpy(char* dest, const char* src)
    {
        memcpy(dest, src, strlen(src));
        return dest;
    }

    char* strncpy(char* dest, const char* src, size_t n)
    {
       for (uint32_t i = 0; i < n; i++) { dest[i] = src[i]; }
        return dest;
    }

    size_t strcspn(const char* str1, const char* str2)
    {
        return 0;
    }

    size_t strlen(const char* str)
    {
        uint32_t len = 0;
        while (str[len] != 0) { len++; }
        return len;
    }

    char* strpbrk(const char* str1, const char* str2)
    {
        return nullptr;
    }

    char* strrchr(const char* str, int c)
    {
        return nullptr;
    }

    size_t strspn(const char* str1, const char* str2)
    {
        return 0;
    }

    char* strstr(const char* haystack, const char* needle)
    {
        return nullptr;
    }

    char* strtok(char* str, const char* delim)
    {
        return nullptr;
    }

    size_t strxfrm(char* dest, const char* str, size_t n)
    {
        return 0;
    }
}