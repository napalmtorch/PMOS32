#include <Kernel/System/Debug.h>
#include <Kernel/Core/Kernel.h>
using namespace HAL;
using namespace Graphics;

EXTC
{
    extern uint32_t _read_cr0();
    extern uint32_t _read_cr2();
    extern uint32_t _read_cr3();
}

namespace System
{
    namespace Debug
    {
        DebugMode CurrentMode;

        void SetMode(DebugMode mode) { CurrentMode = mode; }
        
        DebugMode GetMode() { return CurrentMode; }

        void Break() { asm volatile("xchg %bx, %bx"); }

        bool IsSerial(DebugMode mode) { return (mode == DebugMode::Serial   || mode == DebugMode::All); }
        
        bool IsTerm(DebugMode mode)   { return (mode == DebugMode::Terminal || mode == DebugMode::All); }

        void WriteChar(char c) { WriteChar(c, CurrentMode); }

        void WriteChar(char c, Graphics::Color4 fg) { WriteChar(c, fg, CurrentMode); }

        void WriteChar(char c, DebugMode mode)
        {
            if (IsSerial(mode)) { HAL::Serial::WriteChar(c); }
            if (IsTerm(mode)) { Core::Terminal.WriteChar(c); }
        }

        void WriteChar(char c, Graphics::Color4 fg, DebugMode mode)
        {
            if (IsSerial(mode)) { HAL::Serial::WriteChar(c, fg); }
            if (IsTerm(mode)) { Core::Terminal.WriteChar(c, fg); }
        }
        
        void Write(const char* str) { Write(str, CurrentMode); }

        void Write(const char* str, Graphics::Color4 fg) { Write(str, fg, CurrentMode); }

        void Write(const char* str, DebugMode mode)
        {
            if (IsSerial(mode)) { HAL::Serial::Write(str); }
            if (IsTerm(mode)) { Core::Terminal.Write(str); }
        }

        void Write(const char* str, Graphics::Color4 fg, DebugMode mode)
        {
            if (IsSerial(mode)) { HAL::Serial::Write(str, fg); }
            if (IsTerm(mode)) { Core::Terminal.Write(str, fg); }
        }

        void WriteLine(const char* str) { WriteLine(str, CurrentMode); }

        void WriteLine(const char* str, Graphics::Color4 fg) { WriteLine(str, fg, CurrentMode); }

        void WriteLine(const char* str, DebugMode mode)
        {
            Write(str, mode);
            WriteChar('\n', mode);
        }

        void WriteLine(const char* str, Graphics::Color4 fg, DebugMode mode)
        {
            Write(str, fg, mode);
            WriteChar('\n', mode);
        }

        void PrintFormatted(const char* str, va_list args) { PrintFormatted(str, CurrentMode, args); }
        
        void PrintFormatted(const char* str, ...)
        {
            va_list args;
            va_start(args, str);
            PrintFormatted(str, CurrentMode, args);
            va_end(args);
        }

        void PrintFormatted(const char* str, DebugMode mode, va_list args)
        {
            while (*str != 0)
            {
                if (*str == '%')
                {
                    str++;
                    if (*str == '%') { WriteChar('%', mode); }
                    if (*str == 'c') { WriteChar((char)va_arg(args, int), mode); }
                    else if (*str == 'd')
                    {
                        int dec = va_arg(args, int);
                        char str[32];
                        memset(str, 0, 32);
                        Write(itoa(dec, str, 10), mode);
                    }
                    else if (*str == 'u')
                    {
                        uint32_t dec = va_arg(args, uint32_t);
                        char str[16];
                        memset(str, 0, 16);
                        Write(ltoa(dec, str, 10), mode);
                    }
                    else if (*str == 'x')
                    {
                        uint32_t dec = va_arg(args, uint32_t);
                        char str[16];
                        memset(str, 0, 16);
                        Write(ltoa(dec, str, 16), mode);
                    }
                    else if (*str == '2')
                    {
                        str++;
                        if (*str != 'x' && *str != 'X') { WriteChar(*str, mode); }
                        else
                        {
                            uint32_t num = va_arg(args, uint32_t);
                            char str[16];
                            memset(str, 0, 16);
                            Write(strhex(num, str, false, 1), mode);
                        }
                    }
                    else if (*str == '4')
                    {
                        str++;
                        if (*str != 'x' && *str != 'X') { WriteChar(*str, mode); }
                        else
                        {
                            uint32_t num = va_arg(args, uint32_t);
                            char str[16];
                            memset(str, 0, 16);
                            Write(strhex(num, str, false, 2), mode);
                        }
                    }
                    else if (*str == '8')
                    {
                        str++;
                        if (*str != 'x' && *str != 'X') { WriteChar(*str, mode); }
                        else
                        {
                            uint32_t num = va_arg(args, uint32_t);
                            char str[16];
                            memset(str, 0, 16);
                            Write(strhex(num, str, false, 4), mode);
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
                        Write(val, mode);
                    }
                    else { WriteChar(*str, mode); }
                }
                else { WriteChar(*str, mode); }
                str++;
            }
        }

        void PrintFormatted(const char* str, DebugMode mode, ...)
        {
            va_list args;
            va_start(args, mode);
            PrintFormatted(str, mode, args);
            va_end(args);
        }

        void Header(const char* str, Color4 color, DebugMode mode)
        {
            Write("[", mode);
            Write(str, color, mode);
            Write("] ", mode);
        }

        void Info(const char* str, ...)
        {
            
            va_list args;
            va_start(args, str);
            Header("  >>  ", Color4::Cyan, CurrentMode);
            PrintFormatted(str, CurrentMode, args);
            WriteChar('\n', CurrentMode);
            va_end(args);
            
        }

        void Info(const char* str, DebugMode mode, ...)
        {
            
            va_list args;
            va_start(args, mode);
            Header("  >>  ", Color4::Cyan, mode);
            PrintFormatted(str, mode, args);
            WriteChar('\n', mode);
            va_end(args);
            
        }

        void OK(const char* str, ...)
        {
            
            va_list args;
            va_start(args, str);
            Header("  OK  ", Color4::Green, CurrentMode);
            PrintFormatted(str, CurrentMode, args);
            WriteChar('\n', CurrentMode);
            va_end(args);
            
        }

        void OK(const char* str, DebugMode mode, ...)
        {
            
            va_list args;
            va_start(args, mode);
            Header("  OK  ", Color4::Green, mode);
            PrintFormatted(str, mode, args);
            WriteChar('\n', mode);
            va_end(args);
            
        }

        void Warning(const char* str, ...)
        {
            
            va_list args;
            va_start(args, str);
            Header("  ??  ", Color4::Yellow, CurrentMode);
            PrintFormatted(str, CurrentMode, args);
            WriteChar('\n', CurrentMode);
            va_end(args);
            
        }

        void Warning(const char* str, DebugMode mode, ...)
        {
            
            va_list args;
            va_start(args, mode);
            Header("  ??  ", Color4::Yellow, mode);
            PrintFormatted(str, mode, args);
            WriteChar('\n', mode);
            va_end(args);
            
        }

        void Error(const char* str, ...)
        {
            
            va_list args;
            va_start(args, str);
            Header("  !!  ", Color4::Red, CurrentMode);
            PrintFormatted(str, CurrentMode, args);
            WriteChar('\n', CurrentMode);
            va_end(args);
            
        }

        void Error(const char* str, DebugMode mode, ...)
        {
            
            va_list args;
            va_start(args, mode);
            Header("  !!  ", Color4::Red, mode);
            PrintFormatted(str, mode, args);
            WriteChar('\n', mode);
            va_end(args);
            
        }

        void DumpMemory(void* ptr, uint32_t len)
    {
        int bytes_per_line = 16;
        char temp[16];
        char chars[bytes_per_line];
        memset(temp, 0, 16);
        memset(chars, 0, 16);
        uint8_t* src = (uint8_t*)ptr;

        Write("Dumping memory at: ");
        strhex((uint32_t)src, temp, false, 4);
        Write(temp, Color4::Cyan);
        WriteChar('\n');

        int xx = 0;
        uint32_t pos = 0;
        for (size_t i = 0; i < (len / bytes_per_line); i++)
        {              
            // address range
            pos = i * bytes_per_line;
            chars[0] = '\0';
            strhex((uint32_t)(src + pos), temp, false, 4);
            Write(temp, Color4::Cyan); Write(":");
            strhex((uint32_t)(src + pos + (bytes_per_line - 1)), temp, false, 4);
            Write(temp, Color4::Cyan);
            Write("    ");

            // bytes
            for (size_t j = 0; j < bytes_per_line; j++)
            {
                memset(temp, 0, 16);
                strhex(src[pos + j], temp, false, 1);
                if (src[pos + j] > 0) { Write(temp); }
                else { Write(temp, Color4::Red); }
                Write(" ");

                if (src[pos + j] >= 32 && src[pos + j] <= 126) { stradd(chars, src[pos + j]); }
                else { stradd(chars, '.'); }
            }

            Write("    ");
            Write(chars, Color4::Yellow);
            WriteChar('\n');
        }
    }

        void DumpRegisters(Registers32* regs)
        {
            
            Debug::PrintFormatted("EAX: 0x%8x EBX: 0x%8x ECX: 0x%8x EDX: 0x%8x\n", regs->EAX, regs->EBX, regs->ECX, regs->EDX);
            Debug::PrintFormatted("ESI: 0x%8x EDI: 0x%8x ESP: 0x%8x EBP: 0x%8x\n", regs->ESI, regs->EDI, regs->ESP, regs->EBP);
            Debug::PrintFormatted("EIP: 0x%8x CR0: 0x%8x CR2: 0x%8x CR3: 0x%8x\n", regs->EIP, _read_cr0(), _read_cr2(), _read_cr3());
            
        }
    }
}