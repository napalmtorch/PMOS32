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
        if (w == 0 || h == 0) { return; }
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

    void Image::DrawRect(int x, int y, int w, int h, int thickness, uint32_t color)
    {
        DrawFilledRect(x, y, w, thickness, color);
        DrawFilledRect(x, y + h - thickness, w, thickness, color);
        DrawFilledRect(x, y + thickness, thickness, h - (thickness * 2), color);
        DrawFilledRect(x + w - thickness, y + thickness, thickness, h - (thickness * 2), color);
    }

    void Image::DrawRect3D(int x, int y, int w, int h, bool invert, uint32_t ca, uint32_t cb, uint32_t cc)
    {
        if (invert)
        {
            DrawFilledRect(x, y, w - 1, 1, cc);
            DrawFilledRect(x, y, 1, h - 1, cc);
            DrawFilledRect(x + 1, y + 1, w - 3, 1, cb);
            DrawFilledRect(x + 1, y + 1, 1, h - 3, cb);
            DrawFilledRect(x, y + h - 1, w, 1, ca);
            DrawFilledRect(x + w - 1, y, 1, h, ca);
        }
        else
        {
            DrawFilledRect(x, y, w, 1, ca);
            DrawFilledRect(x, y, 1, h, ca);
            DrawFilledRect(x + 1, y + h - 2, w - 2, 1, cb);
            DrawFilledRect(x + w - 2, y + 1, 1, h - 2, cb);
            DrawFilledRect(x, y + h - 1, w, 1, cc);
            DrawFilledRect(x + w - 1, y, 1, h, cc);
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

    void Image::DrawArray(int x, int y, int w, int h, uint32_t* data)
    {
        if (data == NULL) { return; }
        uint32_t iw = w;
        while (x + w > (int)Width) { w--; }
        while (y + h > (int)Height) { h--; }

        for (int yy = 0; yy < h; yy++)
        {
            uint8_t* src = (uint8_t*)(data + (yy * iw));
            int xx = x;
            if (xx < 0) { xx = 0; }
            while (xx + w > (int)Width) { xx--; }
            uint32_t real_offset = (xx + ((y + yy) * Width)) * 4;
            uint8_t* dest = (uint8_t*)((uint32_t)Data + real_offset);
            //if (y + yy >= (int)Height) { return; }
            if (y + yy >= 0 && (uint32_t)dest >= (uint32_t)Data && (uint32_t)dest < (uint32_t)((uint32_t)Data + (Width * Height * 4)))
            {
                if (x >= 0) { memcpy(dest, src, w * 4); }
                else { memcpy(dest, src - (x * 4), (w + x) * 4); }
            }
        }
    }

    void Image::DrawArray(int x, int y, int w, int h, uint32_t trans, uint32_t* data)
    {
       for (uint32_t i = 0; i < (uint32_t)w * h; i++)
       {
           if (data[i] != trans) { DrawPixel(x + (i % w), y + (i / w), data[i]); }
       }
    }

    uint32_t Image::GetWidth() { return Width; }

    uint32_t Image::GetHeight() { return Height; }

    uint32_t* Image::GetData() { return Data; }
}