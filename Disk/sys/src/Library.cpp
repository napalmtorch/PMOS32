#include <sys/lib/Library.h>
#include <sys/lib/Common.h>

mtable_entry_t* MTABLE_ENTRIES;

int strcmp(const char* str1, const char* str2)
{
    while (*str1)
    {
        if (*str1 != *str2) { break; }
        str1++; str2++;
    }
    return *(char*)str1 - *(char*)str2;
}

void lib_init()
{
    asm volatile("int $0x80": :"a"(0), "b"(0), "c"(0xFF), "d"((unsigned int)&MTABLE_ENTRIES));
}