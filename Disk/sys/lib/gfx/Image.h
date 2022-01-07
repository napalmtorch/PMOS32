#pragma once
#include <sys/lib/Types.h>
#include <sys/lib/Memory.h>
#include <sys/lib/Common.h>
#include <sys/lib/gfx/Font.h>

namespace pmlib
{
    class Image
    {
        private:
            uint32_t  Width;
            uint32_t  Height;
            uint32_t* Data;

        public:
            Image();
            void Create(uint32_t w, uint32_t h);
            void Load(char* path);

        public:
            void Clear(uint32_t color);
            void DrawPixel(int x, int y, uint32_t color);
            void DrawFilledRect(int x, int y, int w, int h, uint32_t color);
            void DrawChar(int x, int y, char c, uint32_t fg, Font font);
            void DrawChar(int x, int y, char c, uint32_t fg, uint32_t bg, Font font);
            void DrawString(int x, int y, char* str, uint32_t fg, Font font);
            void DrawString(int x, int y, char* str, uint32_t fg, uint32_t bg, Font font);
            

        public:
            uint32_t  GetWidth();
            uint32_t  GetHeight();
            uint32_t* GetData();
    };
}