#include <sys/lib/gfx/Graphics.h>
#include <sys/lib/Library.h>
#include <sys/lib/Common.h>

namespace pmlib
{
    void Render(int x, int y, int w, int h, uint32_t* data)
    {
        uint32_t fbw = GetFrameBufferWidth();
        uint32_t fbh = GetFrameBufferHeight();
        uint32_t* fb = GetFrameBuffer();

        if (data == NULL) { return; }
        while (x + w > fbw) { w--; }
        while (y + h > fbh) { h--; }

        for (int yy = 0; yy < h; yy++)
        {
            uint8_t* src = (uint8_t*)(data + (yy * w));
            int xx = x;
            if (xx < 0) { xx = 0; }
            while (xx + w > fbw) { xx--; }
            uint32_t real_offset = (xx + ((y + yy) * fbw)) * 4;
            uint8_t* dest = (uint8_t*)((uint32_t)fb + real_offset);
            if (y + yy >= fbh) { return; }
            if (y + yy >= 0 && (uint32_t)dest >= (uint32_t)fb && (uint32_t)dest < (uint32_t)((uint32_t)fb + (fbw * fbh * 4)))
            {
                if (x >= 0) { memcpy(dest, src, w * 4); }
                else { memcpy(dest, src - (x * 4), (w + x) * 4); }
            }
        }
    }

    uint32_t* GetFrameBuffer() { return (uint32_t*)((MTYPE_VESA_GETFB)MTABLE_ENTRIES[MTYPEID_VESA_GETFB].addr)(); }

    uint32_t GetFrameBufferWidth() { return (uint32_t)((MTYPE_VESA_GETW)MTABLE_ENTRIES[MTYPEID_VESA_GETW].addr)(); }

    uint32_t GetFrameBufferHeight() { return (uint32_t)((MTYPE_VESA_GETW)MTABLE_ENTRIES[MTYPEID_VESA_GETH].addr)(); }
}