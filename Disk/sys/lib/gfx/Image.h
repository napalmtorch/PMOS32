#pragma once
#include <sys/lib/stdint.h>
#include <sys/lib/stdarg.h>
#include <sys/lib/stddef.h>
#include <sys/lib/string.h>
#include <sys/lib/api/Memory.h>
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
            void DrawRect(int x, int y, int w, int h, int thickness, uint32_t color);
            void DrawRect3D(int x, int y, int w, int h, bool invert, uint32_t ca, uint32_t cb, uint32_t cc);
            void DrawChar(int x, int y, char c, uint32_t fg, Font font);
            void DrawChar(int x, int y, char c, uint32_t fg, uint32_t bg, Font font);
            void DrawString(int x, int y, char* str, uint32_t fg, Font font);
            void DrawString(int x, int y, char* str, uint32_t fg, uint32_t bg, Font font);
            void DrawArray(int x, int y, int w, int h, uint32_t* data);
            void DrawArray(int x, int y, int w, int h, uint32_t trans, uint32_t* data);

        public:
            uint32_t  GetWidth();
            uint32_t  GetHeight();
            uint32_t* GetData();
    };
}