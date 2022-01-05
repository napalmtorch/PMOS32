#pragma once
#include <Lib/Types.h>

namespace HAL
{
    namespace Ports
    {
        uint8_t  Read8(uint16_t port);        
        uint16_t Read16(uint16_t port);
        uint32_t Read32(uint16_t port);
        void     ReadString(uint16_t port, uint8_t* data, uint32_t size);

        void Write8(uint16_t port, uint8_t data);
        void Write16(uint16_t port, uint16_t data);
        void Write32(uint16_t port, uint32_t data);
        void WriteString(uint16_t port, uint8_t* data, uint32_t size);
    }
}