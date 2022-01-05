#include <Kernel/HAL/Common/Ports.h>

namespace HAL
{
    namespace Ports
    {
        /// @brief Read 8-bit value from IO port @param port Port value @return Data from IO port
        uint8_t Read8(uint16_t port)
        {
            uint8_t result;
            asm volatile("in %%dx, %%al" : "=a" (result) : "d" (port));
            return result;
        } 

        /// @brief Read 16-bit value from IO port @param port Port value @return Data from IO port
        uint16_t Read16(uint16_t port)
        {
            uint16_t result;
            asm volatile("in %%dx, %%ax" : "=a" (result) : "d" (port));
            return result;
        }

        /// @brief Read 32-bit value from IO port @param port Port value @return Data from IO port
        uint32_t Read32(uint16_t port)
        {
            uint32_t result;
            asm volatile("inl %%dx, %%eax" : "=a" (result) : "dN" (port));
            return result;
        }

        /// @brief Read string from IO port @param port Port value @param data Pointer to buffer @param size Size of buffer
        void ReadString(uint16_t port, uint8_t* data, uint32_t size) { asm volatile("rep insw" : "+D" (data), "+c" (size) : "d" (port) : "memory"); }

        /// @brief Write 8-bit value to IO port @param port Port value @param data Data value
        void Write8(uint16_t port, uint8_t data) { asm volatile("out %%al, %%dx" : : "a" (data), "d" (port)); }

        /// @brief Write 16-bit value to IO port @param port Port value @param data Data value
        void Write16(uint16_t port, uint16_t data) { asm volatile("out %%ax, %%dx" : : "a" (data), "d" (port)); }

        /// @brief Write 32-bit value to IO port @param port Port value @param data Data value
        void Write32(uint16_t port, uint32_t data) { asm volatile("outl %%eax, %%dx" : : "dN" (port), "a" (data)); }
    
        /// @brief Write string to IO port @param port Port value @param data Pointer to buffer @param size Size of buffer
        void WriteString(uint16_t port, uint8_t* data, uint32_t size) { asm volatile("rep outsw" : "+S" (data), "+c" (size) : "d" (port)); }
    }
}