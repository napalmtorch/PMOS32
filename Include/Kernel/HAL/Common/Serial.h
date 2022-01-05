#pragma once
#include <Lib/Types.h>
#include <Kernel/Graphics/Color.h>

namespace HAL
{
    enum class SerialPort
    {
        Disabled,
        COM1 = 0x3F8,
        COM2 = 0x2F8,
        COM3 = 0x3E8,
        COM4 = 0x2E8,
    };

    namespace Serial
    {
        void        SetPort(SerialPort port);
        SerialPort  GetPort();
        const char* GetPortString(SerialPort port);

        char Read();
        void WriteChar(char c);
        void WriteChar(char c, Graphics::Color4 color);
        void Write(const char* str);
        void Write(const char* str, Graphics::Color4 color);
        void WriteLine(const char* str);
        void WriteLine(const char* str, Graphics::Color4 color);
        void SetColor(Graphics::Color4 color);
        void PrintFormattedV(const char* str, va_list args);
        void PrintFormatted(const char* str, ...);
    }
}