#pragma once
#include <Lib/Types.h>
#include <Kernel/Graphics/Color.h>
#include <Kernel/Graphics/Font.h>

namespace HAL
{
    namespace Drivers
    {
        // mode info block
        typedef struct
        {
            uint16_t     Attributes;
            uint8_t      WindowA, WindowB;
            uint16_t     Granularity;
            uint16_t     WindowSize;
            uint16_t     SegmentA, SegmentB;
            uint16_t     WindowFunction[2];
            uint16_t     Pitch, Width, Height;
            uint8_t      CharWidth, CharHeight, Planes, Depth, Banks;
            uint8_t      MemoryModel, BankSize, image_pages;
            uint8_t      Reserved0;
            uint8_t      RedMask, RedPosition;
            uint8_t      GreenMask, GreenPosition;
            uint8_t      BlueMask, BluePosition;
            uint8_t      RSVMask, RSVPosition;
            uint8_t      DirectColor;
            uint32_t     PhysicalBase;
            uint32_t     Reserved1;
            uint16_t     Reserved2;
        } PACKED VESAModeInfo;

        class VESAController
        {
            public:
                uint32_t Width, Height;
                uint32_t CursorX, CursorY;
                
                uint32_t* Buffer;
                VESAModeInfo Info;
                bool     Initialized;

            public:
                void Init();
                void Clear(Graphics::Color color);
                void DrawPixel(uint16_t x, uint16_t y, Graphics::Color color);
                void DrawFilledRect(uint16_t x, uint16_t y, uint16_t w, uint16_t h, Graphics::Color color);
                void DrawChar(uint16_t x, uint16_t y, char c, Graphics::Color fg, Graphics::Color bg, Graphics::Font font);
                void DrawString(uint16_t x, uint16_t y, const char* str, Graphics::Color fg, Graphics::Color bg, Graphics::Font font);
                void Scroll(Graphics::Color bg, Graphics::Font font);
                void EnableCursor();
                void DisableCursor();
                void SetCursor(uint16_t x, uint16_t y, uint16_t w, uint16_t h, Graphics::Color fg, Graphics::Color bg);
        };
    }
}