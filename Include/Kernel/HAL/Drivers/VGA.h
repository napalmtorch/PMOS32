#pragma once
#include <Lib/Types.h>
#include <Kernel/Graphics/Color.h>

namespace HAL
{
    namespace Drivers
    {
        class VGAController
        {
            public:
                uint32_t Width, Height;
                uint8_t* Buffer;
                bool     Initialized;

            public:
                void Init();
                void Clear(Graphics::Color4 fg, Graphics::Color4 bg);
                void PutChar(uint16_t x, uint16_t y, char c, Graphics::Color4 fg, Graphics::Color4 bg);
                void PutString(uint16_t x, uint16_t y, const char* str, Graphics::Color4 fg, Graphics::Color4 bg);
                void Scroll(Graphics::Color4 fg, Graphics::Color4 bg);
                void EnableCursor(uint8_t top, uint8_t bottom);
                void DisableCursor();
                void SetCursor(uint16_t x, uint16_t y);
                uint8_t PackColors(Graphics::Color4 fg, Graphics::Color4 bg);
        };
    }
}