#include <sys/lib/stdio.h>
#include <sys/lib/api/Library.h>

EXTC
{
    int vprintf(const char* str, va_list args)
    {
        ((MTYPE_VPRINTF)MTABLE_ENTRIES[MTYPEID_VPRINTF].addr)(str, args);
        return 0;
    }

    int printf(const char* str, ...)
    {
        va_list args;
        va_start(args, str);
        vprintf(str, args);
        va_end(args);
        return 0;
    }

    int scanf(const char* fmt, ...)
    {
        return 0;
    }
}