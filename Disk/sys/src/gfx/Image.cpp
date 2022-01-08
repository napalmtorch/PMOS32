#include <sys/lib/gfx/Image.h>
#include <sys/lib/api/Library.h>
#include <sys/lib/stdlib.h>
#include <sys/lib/stdio.h>

namespace pmlib
{
    Image::Image()
    {
        Width  = 0;
        Height = 0;
        Data   = nullptr;
    }

    void Image::Create(uint32_t w, uint32_t h)
    {
        if (Data != nullptr) { kfree(Data); }
        Data   = (uint32_t*)kmalloc(w * h * 4, true, MEMTYPE_ARRAY);
        Width  = w;
        Height = h;
        printf("Created %dx%d image\n", w, h);
    }

    void Image::Load(char* path)
    {
        
    }
    
    void Image::Clear(uint32_t color)
    {
        memset(Data, color, Width * Height * 4);
    }

    void Image::DrawPixel(int x, int y, uint32_t color)
    {
        if ((uint32_t)x >= Width || (uint32_t)y >= Height) { return; }
        Data[y * Width + x] = color;
    }

    void Image::DrawFilledRect(int x, int y, int w, int h, uint32_t color)
    {
        for (int yy = 0; yy < h; yy++)
        {
            for (int xx = 0; xx < w; xx++) { DrawPixel(x + xx, y + yy, color); }
        }
    }

    void Image::DrawChar(int x, int y, char c, uint32_t fg, Font font)
    {
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

    void Image::DrawChar(int x, int y, char c, uint32_t fg, uint32_t bg, Font font)
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

    void Image::DrawString(int x, int y, char* str, uint32_t fg, Font font)
    {
        int i = 0, xx = x, yy = y;
        while (str[i] != 0)
        {
            if (str[i] == '\n') { xx = x; yy += font.Height + font.SpacingY; }
            else if (str[i] >= 32 && str[i] <= 126)
            {
                DrawChar(xx, yy, str[i], fg, font);
                xx += font.Width + font.SpacingX;
            }
            i++;
        }
    }

    void Image::DrawString(int x, int y, char* str, uint32_t fg, uint32_t bg, Font font)
    {
        int i = 0, xx = x, yy = y;
        while (str[i] != 0)
        {
            if (str[i] == '\n') { xx = x; yy += font.Height + font.SpacingY; }
            else if (str[i] >= 32 && str[i] <= 126)
            {
                DrawChar(xx, yy, str[i], fg, bg, font);
                xx += font.Width + font.SpacingX;
            }
            i++;
        }
    }

    uint32_t Image::GetWidth() { return Width; }

    uint32_t Image::GetHeight() { return Height; }

    uint32_t* Image::GetData() { return Data; }
}