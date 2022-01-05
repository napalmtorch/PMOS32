#include <Kernel/HAL/Drivers/VESA.h>
#include <Kernel/Core/Kernel.h>
using namespace System;
using namespace Graphics;

namespace HAL
{
    namespace Drivers
    {
        // Initialize VESA controller
        void VESAController::Init()
        {
            if (Initialized) { return; }
            Initialized = true;

            VESAModeInfo* info = (VESAModeInfo*)Kernel::Multiboot.vbe_mode_info;
            memcpy(&Info, info, sizeof(VESAModeInfo));

            Core::VMM.KernelDirectory.Map((uint32_t)Info.PhysicalBase, Info.PhysicalBase, true);
            Width  = Info.Width;
            Height = Info.Height;     
            Buffer = (uint32_t*)Info.PhysicalBase;
            CursorX = 0;
            CursorY = 0;

            Clear(Color::Black);
            Debug::OK("Initialized VESA controller - %dx%d, FB: 0x%8x", Width, Height, (uint32_t)Info.PhysicalBase);
        }

        /// @brief Clear the buffer @param color Color value
        void VESAController::Clear(Color color)
        {
            memset(Buffer, (uint32_t)color, Width * Height * 4);
        }

        /// @brief Put pixel at position in buffer @param x X coordinate @param y Y coordinate @param color Color value
        void VESAController::DrawPixel(uint16_t x, uint16_t y, Color color)
        {
            if (x >= Width || y >= Height) { return; }
            Buffer[y * Width + x] = (uint32_t)color;
        }

        /// @brief Put filled rectangle at position in buffer @param x X coordinate @param y Y coordinate @param w Width @param h Height @param color Color Value
        void VESAController::DrawFilledRect(uint16_t x, uint16_t y, uint16_t w, uint16_t h, Color color)
        {
            for (uint16_t yy = 0; yy < h; yy++)
            {
                for (uint16_t xx = 0; xx < w; xx++)
                {
                    DrawPixel(x + xx, y + yy, color);
                }
            }
        }

        /// @brief Put character at position in buffer @param x X coordinate @param y Y coordinate @param c Character value @param fg Fore color @param bg Back color @param font Font
        void VESAController::DrawChar(uint16_t x, uint16_t y, char c, Color fg, Color bg, Font font)
        {
            DrawFilledRect(x, y, font.Width + font.SpacingX, font.Height + font.SpacingY, bg);
            uint32_t p = font.Height * c;
            for (size_t cy = 0; cy < font.Height; cy++)
            {
                for (size_t cx = 0; cx < font.Width; cx++)
                {
                    uint32_t xx = x + (font.Width - cx);
                    uint32_t yy = y + cy;
                    if (GetBitAddressFromByte(font.Data[p + cy], cx + 1)) { DrawPixel(xx, yy, fg); }
                }
            }
        }

        /// @brief Put string at position in buffer @param x X coordinate @param y Y coordinate @param str String value @param fg Fore color @param bg Back color @param font Font
        void VESAController::DrawString(uint16_t x, uint16_t y, const char* str, Color fg, Color bg, Font font)
        {
            int i = 0, xx = x, yy = y;
            while (str[i] != 0)
            {
                if (str[i] == '\n') { xx = x; yy += font.Height + font.SpacingY; }
                else if (isprint(str[i]))
                {
                    DrawChar(xx, yy, str[i], fg, bg, font);
                    xx += font.Width + font.SpacingX;
                }
                i++;
            }
        }

        /// @brief Scroll screen by height of font @param bg Back color @param font Font
        void VESAController::Scroll(Color bg, Graphics::Font font)
        {
            // calculate required values to perform scroll
            uint32_t line = (Width * 4) * 16;
            uint32_t size = Width * Height * 4;
            uint8_t* dest = (uint8_t*)Buffer;
            uint8_t* src = (uint8_t*)((uint32_t)Buffer + line);

            // move all data up 1 line
            memcpy(dest, src, size - line);

            // clear bottom line
            DrawFilledRect(0, Height - (font.Height + font.SpacingY), Width, font.Height + font.SpacingY, bg);
            //for (uint16_t i = 0; i < Width; i++) { DrawChar(i * (font.Width + font.SpacingX), (Height - 1) * (font.Height + font.SpacingY), 0x20, bg, bg, font); }
        }

        // Enable cursor
        void VESAController::EnableCursor() { }

        // Disable cursor  
        void VESAController::DisableCursor() { }

        /// @brief Set cursor dimensions and colors @param x X coordinate @param y Y coordinate @param w Width @param h Height @param fg Cursor color @param bg Back color
        void VESAController::SetCursor(uint16_t x, uint16_t y, uint16_t w, uint16_t h, Graphics::Color fg, Graphics::Color bg)
        {
            CursorX = x;
            CursorY = y;
            DrawFilledRect(CursorX, CursorY, w, h, fg);
        }
    }
}