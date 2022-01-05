#include <Kernel/HAL/Drivers/VGA.h>
#include <Kernel/Core/Kernel.h>
using namespace System;
using namespace Graphics;

#define VGA_DEFAULT_ADDR   0xC00B8000
#define VGA_DEFAULT_WIDTH  80
#define VGA_DEFAULT_HEIGHT 25

namespace HAL
{
    namespace Drivers
    {
        // Initialize VGA controller
        void VGAController::Init()
        {
            if (Initialized) { return; }

            Buffer      = (uint8_t*)VGA_DEFAULT_ADDR;
            Width       = VGA_DEFAULT_WIDTH;
            Height      = VGA_DEFAULT_HEIGHT;
            Initialized = true;

            Clear(Color4::White, Color4::Black);
            EnableCursor(0, 15);
            SetCursor(0, 0);
            Debug::OK("Initialized VGA controller at %dx%d", Width, Height);
        }

        /// @brief Clear the buffer @param fg Fore color @param bg Back color
        void VGAController::Clear(Color4 fg, Color4 bg)
        {
            for (uint32_t i = 0; i < Width * Height * 2; i += 2)
            {
                Buffer[i + 0] = 0x20;
                Buffer[i + 1] = PackColors(fg, bg);
            }
            SetCursor(0, 0);
        }

        /// @brief Put character at position in buffer @param x X coordinate @param y Y coordinate @param c Character value @param fg Fore color @param bg Back color
        void VGAController::PutChar(uint16_t x, uint16_t y, char c, Color4 fg, Color4 bg)
        {
            if (x >= Width || y >= Height) { return; }
            uint32_t offset = (y * Width + x) * 2;
            Buffer[offset + 0] = c;
            Buffer[offset + 1] = PackColors(fg, bg);
        }
    
        /// @brief Put string at position in buffer @param x X coordinate @param y Y coordinate @param str String value @param fg Fore color @param bg Back color
        void VGAController::PutString(uint16_t x, uint16_t y, const char* str, Color4 fg, Color4 bg)
        {
            uint32_t i = 0;
            uint16_t xx = x, yy = y;

            while (str[i] != 0)
            {
                if (str[i] == '\n') { xx = x; yy++; }
                else if (isprint(str[i]))
                {
                    PutChar(xx, yy, str[i], fg, bg);
                    xx++;
                    if (xx >= Width) { xx = x; yy++; }
                }
                i++;
            }
        }

        /// @brief Scroll by 1 line @param fg Fore color @param bg Back color
        void VGAController::Scroll(Color4 fg, Color4 bg)
        {
            // calculate required values to perform scroll
            uint32_t line = Width * 2;
            uint32_t size = Width * Height * 2;
            uint8_t* dest = (uint8_t*)Buffer;
            uint8_t* src = (uint8_t*)((uint32_t)Buffer + line);

            // move all data up 1 line
            memcpy(dest, src, size - line);

            // clear bottom line
            for (uint16_t i = 0; i < Width; i++) { PutChar(i, Height - 1, 0x20, fg, bg); }
        }

        /// @brief Enable cursor with specified size @param top Top position @param bottom Bottom position
        void VGAController::EnableCursor(uint8_t top, uint8_t bottom)
        {
            Ports::Write8(0x3D4, 0x0A);
            Ports::Write8(0x3D5, (Ports::Read8(0x3D5) & 0xC0) | top);
        
            Ports::Write8(0x3D4, 0x0B);
            Ports::Write8(0x3D5, (Ports::Read8(0x3D5) & 0xE0) | bottom);
        }

        // Disable cursor
        void VGAController::DisableCursor()
        {
            Ports::Write8(0x3D4, 0x0A);
	        Ports::Write8(0x3D5, 0x20);
        }

        /// @brief Set cursor position @param x X coordinate @param y Y coordinate
        void VGAController::SetCursor(uint16_t x, uint16_t y)
        {
            uint16_t pos = y * Width + x;
            Ports::Write8(0x3D4, 0x0F);
            Ports::Write8(0x3D5, (uint8_t)((pos & 0xFF)));
            Ports::Write8(0x3D4, 0x0E);
            Ports::Write8(0x3D5, (uint8_t)((pos >> 8) & 0xFF));
        }

        /// @brief Pack 2 colors into 1 VGA attribute byte @param fg Fore color @param bg Back color @return Packed value
        uint8_t VGAController::PackColors(Color4 fg, Color4 bg) { return (uint8_t)(((uint8_t)bg << 4) | (uint8_t)fg); }
    }
}