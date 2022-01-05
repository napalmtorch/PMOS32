#pragma once
#include <Lib/Types.h>

namespace HAL
{
    typedef struct
    {
        uint16_t LimitLow;
        uint16_t BaseLow;
        uint8_t  BaseMiddle;
        uint8_t  Access;
        uint8_t  Granularity;
        uint8_t  BaseHigh;
    } PACKED GDTEntry;

    typedef struct
    {
        uint16_t Limit;
        uint32_t Base;
    } PACKED GDTRegister;

    namespace GDT
    {
        void Init();
        void SetDescriptor(uint32_t num, uint32_t base, uint32_t limit, uint8_t access, uint8_t gran);
    }
}