#pragma once

#define EXTC extern "C"

EXTC
{
    #define PACKED __attribute__((packed))
    #define UNUSED(x) (void)(x)
    #define NULL 0

    typedef signed char          int8_t;
    typedef signed short         int16_t;
    typedef signed int           int32_t;
    typedef signed long long     int64_t;

    typedef unsigned char        uint8_t;
    typedef unsigned short       uint16_t;
    typedef unsigned int         uint32_t;
    typedef unsigned long long   uint64_t;

    typedef unsigned int         size_t;

    #define INT8_MIN  -0x80
    #define INT16_MIN -0x8000U
    #define INT32_MIN -0x80000000U
    #define INT64_MIN -0x8000000000000000L

    #define INT8_MAX  0x7FU
    #define INT16_MAX 0x7FFFU
    #define INT32_MAX 0x7FFFFFFFU
    #define INT64_MAX 0x7FFFFFFFFFFFFFFFL

    #define UINT8_MAX  0xFFU
    #define UINT16_MAX 0xFFFFU
    #define UINT32_MAX 0xFFFFFFFFU
    #define UINT64_MAX 0xFFFFFFFFFFFFFFFFU

    typedef char* va_list;

    #define __va_argsiz(t)	\
        (((sizeof(t) + sizeof(int) - 1) / sizeof(int)) * sizeof(int))

    #define va_start(ap, pN)	\
        ((ap) = ((va_list) (&pN) + __va_argsiz(pN)))

    #define va_end(ap)	((void)0)

    #define va_arg(ap, t)					\
        (((ap) = (ap) + __va_argsiz(t)),		\
        *((t*) (void*) ((ap) - __va_argsiz(t))))

    #define _low16(address) (uint16_t)((address) & 0xFFFF)
    #define _high16(address) (uint16_t)(((address) & 0xFFFF0000) >> 16)
}