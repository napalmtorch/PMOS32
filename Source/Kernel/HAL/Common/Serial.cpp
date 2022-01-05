#include <Kernel/HAL/Common/Serial.h>
#include <Kernel/Core/Kernel.h>
using namespace Graphics;

namespace HAL
{
    namespace Serial
    {
        // Current serial port
        SerialPort CurrentPort = SerialPort::Disabled;

        /// @brief Set current serial port @param port Port value
        void SetPort(SerialPort port) 
        { 
            CurrentPort = port; 
            uint16_t pnum = (uint16_t)port;

            // send port data to serial controller
            Ports::Write8(pnum + 1, 0x00);    // disable all interrupts
            Ports::Write8(pnum + 3, 0x80);    // set baud rate divisor
            Ports::Write8(pnum + 0, 0x03);    // set divisor to 3 (lo uint8_t) 38400 baud
            Ports::Write8(pnum + 1, 0x00);    // hi uint8_t
            Ports::Write8(pnum + 3, 0x03);    // 8 bits, no parity, one stop bit
            Ports::Write8(pnum + 2, 0xC7);    // enable fifo, clear them, with 14-uint8_t threshold
            Ports::Write8(pnum + 4, 0x0B);    // irqs enabled, rts/dsr set
            Ports::Write8(pnum + 4, 0x1E);    // set in loopback mode, test the serial chip
            Ports::Write8(pnum + 0, 0xAE);    // test serial chip (send uint8_t 0xAE and check if serial returns same uint8_t)

            // check if serial is faulty
            if (Ports::Read8(pnum) != 0xAE) { CurrentPort = SerialPort::Disabled; return; }
            
            // ff serial is not faulty set it in normal operation mode
            Ports::Write8(pnum + 4, 0x0F);
        }

        /// @brief Get current serial port @return Port value
        SerialPort GetPort() { return CurrentPort; }
        
        /// @brief Get string representation of serial port @param Port value @return String representation
        const char* GetPortString(SerialPort port)
        {
            switch (port)
            {
                case SerialPort::Disabled: { return "DISABLED"; }
                case SerialPort::COM1: { return "COM1"; }
                case SerialPort::COM2: { return "COM2"; }
                case SerialPort::COM3: { return "COM3"; }
                case SerialPort::COM4: { return "COM4"; }
                default:               { return "ERROR"; }
            }
            return nullptr;
        }

        /// @brief Check if serial has recieved data @return Recieved state
        bool HasRecieved()
        {
            if (CurrentPort == SerialPort::Disabled) { return false; }
            return Ports::Read8((uint16_t)CurrentPort + 5) & 1;
        }

        /// @brief Check if serial can send data @return Sent state
        bool CanSend()
        {
            if (CurrentPort == SerialPort::Disabled) { return false; }
            return Ports::Read8((uint16_t)CurrentPort + 5) & 0x20;
        }

        /// @brief Read character from serial port @return Character
        char Read()
        {
            if (CurrentPort == SerialPort::Disabled) { return false; }
            while (!HasRecieved());
            return Ports::Read8((uint16_t)CurrentPort);
        }

        /// @brief Write character to serial port @param c Character value
        void WriteChar(char c)
        {
            if (CurrentPort == SerialPort::Disabled) { return; }
            while (!CanSend());
            Ports::Write8((uint16_t)CurrentPort, c);
        }
        
        /// @brief Write character to serial port @param c Character value @param color Character color
        void WriteChar(char c, Color4 color)
        {
            if (CurrentPort == SerialPort::Disabled) { return; }
            SetColor(color);
            WriteChar(c);
            SetColor(Color4::White);
        }

        /// @brief Write string to serial port @param str String value
        void Write(const char* str)
        {
            if (CurrentPort == SerialPort::Disabled) { return; }
            int i = 0;
            while (str[i] != 0) { WriteChar(str[i]); i++; }
        }
        
        /// @brief Write string to serial port @param str String value @param color String color
        void Write(const char* str, Color4 color)
        {
            if (CurrentPort == SerialPort::Disabled) { return; }
            SetColor(color);
            Write(str);
            SetColor(Color4::White);
        }

        /// @brief Write line to serial port @param str String value
        void WriteLine(const char* str)
        {
            if (CurrentPort == SerialPort::Disabled) { return; }
            Write(str);
            WriteChar('\n');
        }

        /// @brief Write line to serial port @param str String value @param color String color
        void WriteLine(const char* str, Color4 color)
        {
            if (CurrentPort == SerialPort::Disabled) { return; }
            Write(str, color);
            WriteChar('\n');
        }

        /// @brief Send color to serial port @param color Color value
        void SetColor(Graphics::Color4 color)
        {
            if (CurrentPort == SerialPort::Disabled) { return; }

            uint8_t c = (uint8_t)color;
            switch (c)
            {
                case 0x0: { Write("\033[34");  break; }
                case 0x1: { Write("\033[34m"); break; }
                case 0x2: { Write("\033[32m"); break; }
                case 0x3: { Write("\033[36m"); break; }
                case 0x4: { Write("\033[31m"); break; }
                case 0x5: { Write("\033[35m"); break; }
                case 0x6: { Write("\033[33m"); break; }
                case 0x7: { Write("\033[37m"); break; }
                case 0x8: { Write("\033[37m"); break; }
                case 0x9: { Write("\033[34m"); break; }
                case 0xA: { Write("\033[32m"); break; }
                case 0xB: { Write("\033[36m"); break; }
                case 0xC: { Write("\033[31m"); break; }
                case 0xD: { Write("\033[35m"); break; }
                case 0xE: { Write("\033[33m"); break; }
                case 0xF: { Write("\033[37m"); break; }
                default:  { Write("\033[0m");  break; }
            }
        }

        /// @brief Print formatted string using variadic list @param str Formatted string value @param va_list Arguments
        void PrintFormattedV(const char* str, va_list args)
        {
            if (CurrentPort == SerialPort::Disabled) { return; }
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

        /// @brief Print formatted string with arguments @param str Formatted string value @param ... Arguments
        void PrintFormatted(const char* str, ...)
        {
            if (CurrentPort == SerialPort::Disabled) { return; }
            va_list args;
            va_start(args, str);
            PrintFormattedV(str, args);
            va_end(args);
        }
    }
}