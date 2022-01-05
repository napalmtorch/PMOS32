#pragma once
#include <Lib/Types.h>
#include <Kernel/Graphics/Color.h>

namespace System
{
    enum class TerminalType : uint8_t
    {
        VGA,
        VESA,
        VESABuffered,
    };

    class TerminalManager
    {
        public:
            uint32_t Width, Height;
            uint32_t CursorX, CursorY;
            Graphics::Color4 ForeColor, BackColor;
            TerminalType Type;

        public:
            void Init(TerminalType type);
            
        public:
            void Clear();
            void Clear(Graphics::Color4 bg);
            void NewLine(int amount = 1);
            void Delete(int amount = 1);
            void Scroll(int amount = 1);
            void PutChar(int x, int y, char c, Graphics::Color4 fg, Graphics::Color4 bg);

        public:
            void WriteChar(char c);
            void WriteChar(char c, Graphics::Color4 fg);
            void WriteChar(char c, Graphics::Color4 fg, Graphics::Color4 bg);
            void Write(const char* str);
            void Write(const char* str, Graphics::Color4 fg);
            void Write(const char* str, Graphics::Color4 fg, Graphics::Color4 bg);
            void WriteLine(const char* str);
            void WriteLine(const char* str, Graphics::Color4 fg);
            void WriteLine(const char* str, Graphics::Color4 fg, Graphics::Color4 bg);
            void PrintFormattedV(const char* str, va_list args);
            void PrintFormatted(const char* str, ...);

        public:
            void SetCursorPos(int x, int y);
            void SetCursorX(int x);
            void SetCursorY(int y);
            void SetColors(Graphics::Color4 fg, Graphics::Color4 bg);
            void SetForeColor(Graphics::Color4 fg);
            void SetBackColor(Graphics::Color4 bg);

        public:
            uint32_t GetCursorX();
            uint32_t GetCursorY();
            Graphics::Color4 GetForeColor();
            Graphics::Color4 GetBackColor();
    };
}