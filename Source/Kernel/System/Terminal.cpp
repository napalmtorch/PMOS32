#include <Kernel/System/Terminal.h>
#include <Kernel/Core/Kernel.h>
using namespace HAL;
using namespace Graphics;

namespace System
{
    void TerminalManager::Init(TerminalType type)
    {
        Type = type;
        if (type == TerminalType::VGA) { Width = Core::VGA.Width; Height = Core::VGA.Height; }
        else if (type == TerminalType::VESA) { Width = Core::VESA.Width / 8; Height = Core::VESA.Height / 16; }

        SetColors(Color4::White, Color4::Black);
        Clear();

        Debug::OK("Initialized terminal");
    }
    
    void TerminalManager::Clear() { Clear(BackColor); }

    void TerminalManager::Clear(Color4 bg)
    {
        BackColor = bg;
        if (Type == TerminalType::VGA) { Core::VGA.Clear(ForeColor, BackColor); }
        else if (Type == TerminalType::VESA) { Core::VESA.Clear(ConvertColor4ToColor(BackColor)); }
        SetCursorPos(0, 0);
    }

    void TerminalManager::NewLine(int amount)
    {
        if (Type == TerminalType::VESA) { Core::VESA.DrawFilledRect(CursorX * 8, CursorY * 16, 8, 16, ConvertColor4ToColor(BackColor)); }
        SetCursorPos(0, CursorY + 1);
        if (CursorY >= Height) { Scroll(); }
    }

    void TerminalManager::Delete(int amount)
    {
        if (CursorX >= 0)
        {
            if (Type == TerminalType::VESA) { Core::VESA.DrawFilledRect(CursorX * 8, CursorY * 16, 8, 16, ConvertColor4ToColor(BackColor)); }
            SetCursorX(CursorX - 1);
            PutChar(CursorX, CursorY, 0x20, ForeColor, BackColor);
            if (Type == TerminalType::VESA) { Core::VESA.SetCursor(CursorX * 8, CursorY * 16, 8, 16, ConvertColor4ToColor(ForeColor), ConvertColor4ToColor(BackColor)); }
        }
        else if (CursorY >= 0)
        {
            if (Type == TerminalType::VESA) { Core::VESA.DrawFilledRect(CursorX * 8, CursorY * 16, 8, 16, ConvertColor4ToColor(BackColor)); }
            SetCursorPos(Width - 1, CursorY - 1);
            PutChar(CursorX, CursorY, 0x20, ForeColor, BackColor);
            if (Type == TerminalType::VESA) { Core::VESA.SetCursor(CursorX * 8, CursorY * 16, 8, 16, ConvertColor4ToColor(ForeColor), ConvertColor4ToColor(BackColor)); }
        }
    }

    void TerminalManager::Scroll(int amount)
    {
        if (Type == TerminalType::VGA) { Core::VGA.Scroll(ForeColor, BackColor); }
        else if (Type == TerminalType::VESA) { Core::VESA.Scroll(ConvertColor4ToColor(BackColor), Fonts::Serif8x16); }

        SetCursorPos(0, Height - 1);
    }

    void TerminalManager::PutChar(int x, int y, char c, Color4 fg, Color4 bg)
    {
        if (Type == TerminalType::VGA) { Core::VGA.PutChar(x, y, c, fg, bg); }
        else if (Type == TerminalType::VESA) { Core::VESA.DrawChar(x * 8, y * 16, c, ConvertColor4ToColor(fg), ConvertColor4ToColor(bg), Fonts::Serif8x16); }
    }

    void TerminalManager::WriteChar(char c) { WriteChar(c, ForeColor, BackColor); }

    void TerminalManager::WriteChar(char c, Color4 fg) { WriteChar(c, fg, BackColor); }

    void TerminalManager::WriteChar(char c, Color4 fg, Color4 bg)
    {
        if (c == '\n') { NewLine(); }
        else if (isprint(c))
        {
            PutChar(CursorX, CursorY, c, fg, bg);
            SetCursorX(CursorX + 1);
            if (CursorX >= Width) { NewLine(); }
        }
    }

    void TerminalManager::Write(const char* str) { Write(str, ForeColor, BackColor); }

    void TerminalManager::Write(const char* str, Color4 fg) { Write(str, fg, BackColor); }

    void TerminalManager::Write(const char* str, Color4 fg, Color4 bg)
    {
        int i = 0;
        while (str[i] != 0) { WriteChar(str[i], fg, bg); i++; }
    }

    void TerminalManager::WriteLine(const char* str) { WriteLine(str, ForeColor, BackColor); }
    
    void TerminalManager::WriteLine(const char* str, Color4 fg) { WriteLine(str, fg, BackColor); }

    void TerminalManager::WriteLine(const char* str, Color4 fg, Color4 bg)
    {
        int i = 0;
        while (str[i] != 0) { WriteChar(str[i], fg, bg); i++; }
        NewLine();
    }

    void TerminalManager::PrintFormattedV(const char* str, va_list args)
    {
        while (*str != 0)
        {
            if (*str == '%')
            {
                str++;
                if (*str == '%') { WriteChar('%'); }
                if (*str == 'c') { WriteChar((char)va_arg(args, int)); }
                else if (*str == 'd' || *str == 'u')
                {
                    int dec = va_arg(args, int);
                    char str[16];
                    memset(str, 0, 16);
                    Write(itoa(dec, str, 10));
                }
                else if (*str == 'u')
                {
                    uint32_t dec = va_arg(args, uint32_t);
                    char str[16];
                    memset(str, 0, 16);
                    Write(ltoa(dec, str, 10));
                }
                else if (*str == 'x')
                {
                    uint32_t dec = va_arg(args, uint32_t);
                    char str[16];
                    memset(str, 0, 16);
                    Write(ltoa(dec, str, 16));
                }
                else if (*str == '2')
                {
                    str++;
                    if (*str != 'x' && *str != 'X') { WriteChar(*str); }
                    else
                    {
                        uint32_t num = va_arg(args, uint32_t);
                        char str[16];
                        memset(str, 0, 16);
                        Write(strhex(num, str, false, 1));
                    }
                }
                else if (*str == '4')
                {
                    str++;
                    if (*str != 'x' && *str != 'X') { WriteChar(*str); }
                    else
                    {
                        uint32_t num = va_arg(args, uint32_t);
                        char str[16];
                        memset(str, 0, 16);
                        Write(strhex(num, str, false, 2));
                    }
                }
                else if (*str == '8')
                {
                    str++;
                    if (*str != 'x' && *str != 'X') { WriteChar(*str); }
                    else
                    {
                        uint32_t num = va_arg(args, uint32_t);
                        char str[16];
                        memset(str, 0, 16);
                        Write(strhex(num, str, false, 4));
                    }
                }
                else if (*str == '1')
                {
                    str++;
                    if (*str != '6') { WriteChar(*str); continue; }
                    str++;
                    if (*str != 'x' && *str != 'X') { WriteChar(*str); continue; }
                    uint64_t num = va_arg(args, uint64_t);
                    char str[32];
                    memset(str, 0, 32);
                    Write(strhex((uint32_t)((num & 0xFFFFFFFF00000000) >> 32), str, false, 4));
                    Write(strhex((uint32_t)((num & 0x00000000FFFFFFFF)), str, false, 4));
                }
                else if (*str == 's')
                {
                    char* val = va_arg(args, char*);
                    Write(val);
                }
                else { WriteChar(*str); }
            }
            else { WriteChar(*str); }
            str++;
        }
    }

    void TerminalManager::PrintFormatted(const char* str, ...)
    {
        va_list args;
        va_start(args, str);
        PrintFormattedV(str, args);
        va_end(args);
    }

    void TerminalManager::SetCursorPos(int x, int y) 
    { 
        CursorX = x;
        CursorY = y;
        if (Type == TerminalType::VGA) { Core::VGA.SetCursor(CursorX, CursorY); }
        else if (Type == TerminalType::VESA) { Core::VESA.SetCursor(CursorX * 8, CursorY * 16, 8, 16, ConvertColor4ToColor(ForeColor), ConvertColor4ToColor(BackColor)); }
    }
    
    void TerminalManager::SetCursorX(int x)
    {
        CursorX = x; 
        if (Type == TerminalType::VGA) { Core::VGA.SetCursor(CursorX, CursorY); }
        else if (Type == TerminalType::VESA) { Core::VESA.SetCursor(CursorX * 8, CursorY * 16, 8, 16, ConvertColor4ToColor(ForeColor), ConvertColor4ToColor(BackColor)); }
    }
    
    void TerminalManager::SetCursorY(int y)
    {
        CursorY = y;
        if (Type == TerminalType::VGA) { Core::VGA.SetCursor(CursorX, CursorY); }
        else if (Type == TerminalType::VESA) { Core::VESA.SetCursor(CursorX * 8, CursorY * 16, 8, 16, ConvertColor4ToColor(ForeColor), ConvertColor4ToColor(BackColor)); }
    }
    
    void TerminalManager::SetColors(Color4 fg, Color4 bg) { ForeColor = fg; BackColor = bg; }
    
    void TerminalManager::SetForeColor(Color4 fg) { ForeColor = fg; }
    
    void TerminalManager::SetBackColor(Color4 bg) { BackColor = bg; }

    uint32_t TerminalManager::GetCursorX() { return CursorX; }

    uint32_t TerminalManager::GetCursorY() { return CursorY; }
    
    Color4 TerminalManager::GetForeColor() { return ForeColor; }
    
    Color4 TerminalManager::GetBackColor() { return BackColor; }
}