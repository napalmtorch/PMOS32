#pragma once
#include <sys/lib/stdint.h>
#include <sys/lib/stdarg.h>
#include <sys/lib/stddef.h>

namespace pmlib
{
    typedef struct
    {
        uint16_t Millisecond;
        uint8_t  Second;
        uint8_t  Minute;
        uint8_t  Hour;
        uint8_t  Day;
        uint8_t  Month;
        uint16_t Year;
    } PACKED DateTime;

    DateTime GetCurrentTime();
}