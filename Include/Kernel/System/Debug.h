#pragma once
#include <Lib/Types.h>
#include <Kernel/Graphics/Color.h>
#include <Kernel/HAL/Interrupts/IDT.h>

namespace System
{
    enum class DebugMode
    {
        Disabled,
        Serial,
        Terminal,
        All,
    };

    namespace Debug
    {
        void      SetMode(DebugMode mode);
        DebugMode GetMode();

        void Break();

        void WriteChar(char c);
        void WriteChar(char c, Graphics::Color4 fg);
        void WriteChar(char c, DebugMode mode);
        void WriteChar(char c, Graphics::Color4 fg, DebugMode mode);
        
        void Write(const char* str);
        void Write(const char* str, Graphics::Color4 fg);
        void Write(const char* str, DebugMode mode);
        void Write(const char* str, Graphics::Color4 fg, DebugMode mode);

        void WriteLine(const char* str);
        void WriteLine(const char* str, Graphics::Color4 fg);
        void WriteLine(const char* str, DebugMode mode);
        void WriteLine(const char* str, Graphics::Color4 fg, DebugMode mode);

        void PrintFormatted(const char* str, va_list args);
        void PrintFormatted(const char* str, ...);
        void PrintFormatted(const char* str, DebugMode mode, va_list args);
        void PrintFormatted(const char* str, DebugMode mode, ...);

        void Header(const char* str, Graphics::Color4 color, DebugMode mode);
        void Info(const char* str, ...);
        void Info(const char* str, DebugMode mode, ...);
        void OK(const char* str, ...);
        void OK(const char* str, DebugMode mode, ...);
        void Warning(const char* str, ...);
        void Warning(const char* str, DebugMode mode, ...);
        void Error(const char* str, ...);
        void Error(const char* str, DebugMode mode, ...);

        void DumpRegisters(HAL::Registers32* regs);
    }
}