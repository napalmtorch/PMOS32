#pragma once
#include <Lib/Types.h>
#include <Lib/Time.h>
#include <Kernel/HAL/Memory/Heap.h>
#include <Kernel/System/Process.h>

namespace System
{
    typedef struct
    {
        const char* Name;
        uint32_t    Address;
    } PACKED MethodEntry;

    namespace ExternalMethods
    {
        void* MEM_ALLOCATE(uint32_t size, bool clear, HAL::Memory::HeapType type);
        void  MEM_FREE(void* ptr);
        void  VPRINTF(const char* str, va_list args);

        void VESA_CLEAR(uint32_t color);
        void VESA_BLIT(uint32_t x, uint32_t y, uint32_t color);
        void VESA_RECTF(uint32_t x, uint32_t y, uint32_t w, uint32_t h, uint32_t color);
        uint32_t* VESA_GETFB();
        uint32_t  VESA_GETW();
        uint32_t  VESA_GETH();

        bool KB_DOWN(uint8_t key);
        bool KB_UP(uint8_t key);
        uint32_t MS_GETX();
        uint32_t MS_GETY();
        bool     MS_GETLEFT();
        bool     MS_GETRIGHT();

        DateTime GET_TIME();

        bool SEND_MSG(char* name, ProcessMessage msg);
        ProcessMessage RECV_MSG();
        bool MSG_READY();

        void YIELD();
    }

    namespace MethodLibrary
    {
        extern MethodEntry* Entries;
        extern uint32_t Count;

        void Init();
        void Register(int i, const char* name, uint32_t addr);
    }
}