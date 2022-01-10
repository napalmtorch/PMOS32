#include <Kernel/System/Methods.h>
#include <Kernel/Core/Kernel.h>
using namespace HAL;
using namespace Graphics;

namespace System
{
    namespace MethodLibrary
    {
        MethodEntry* Entries;
        uint32_t Count;

        int GetFreeIndex();

        void Init()
        {
            Count   = 0;
            Entries = (MethodEntry*)Core::Heap.Allocate(sizeof(MethodEntry) * 4096, true, Memory::HeapType::Array);

            Register(1, "MEM_ALLOCATE", (uint32_t)ExternalMethods::MEM_ALLOCATE);
            Register(2, "MEM_FREE", (uint32_t)ExternalMethods::MEM_FREE);
            Register(3, "VPRINTF", (uint32_t)ExternalMethods::VPRINTF);

            Register(4, "VESA_CLEAR", (uint32_t)ExternalMethods::VESA_CLEAR);
            Register(5, "VESA_BLIT", (uint32_t)ExternalMethods::VESA_BLIT);
            Register(6, "VESA_RECTF", (uint32_t)ExternalMethods::VESA_RECTF);
            Register(7, "VESA_GETFB", (uint32_t)ExternalMethods::VESA_GETFB);
            Register(8, "VESA_GETW", (uint32_t)ExternalMethods::VESA_GETW);
            Register(9, "VESA_GETH", (uint32_t)ExternalMethods::VESA_GETH);

            Register(10, "KB_DOWN", (uint32_t)ExternalMethods::KB_DOWN);
            Register(11, "KB_UP", (uint32_t)ExternalMethods::KB_UP);
            Register(12, "MS_GETX", (uint32_t)ExternalMethods::MS_GETX);
            Register(13, "MS_GETY", (uint32_t)ExternalMethods::MS_GETY);
            Register(14, "MS_GETLEFT", (uint32_t)ExternalMethods::MS_GETLEFT);
            Register(15, "MS_GETRIGHT", (uint32_t)ExternalMethods::MS_GETRIGHT);

            Register(16, "GET_TIME", (uint32_t)ExternalMethods::GET_TIME);

            Register(17, "SEND_MSG", (uint32_t)ExternalMethods::SEND_MSG);
            Register(18, "RECV_MSG", (uint32_t)ExternalMethods::RECV_MSG);
            Register(19, "MSG_READY", (uint32_t)ExternalMethods::MSG_READY);

            Register(20, "YIELD", (uint32_t)ExternalMethods::YIELD);

            Debug::OK("Initialized method library");
        }

        void Register(int i, const char* name, uint32_t addr)
        {
            if (i < 0 || i >= 4096) { Debug::Error("Maximum amount of registered methods reached"); return; }
            Entries[i].Name    = name;
            Entries[i].Address = addr;
            Count++;
            Debug::Info("Registered method - ADDR: 0x%8x, NAME: '%s'", addr, name);
        }

        int GetFreeIndex()
        {
            for (uint32_t i = 0; i < 4096; i++)
            {
                if (Entries[i].Address == 0 && Entries[i].Name == nullptr) { return i; }
            }
            return -1;
        }
    }

    namespace ExternalMethods
    {
        void* MEM_ALLOCATE(uint32_t size, bool clear, HAL::Memory::HeapType type) { return Core::Heap.Allocate(size, clear, type); }

        void MEM_FREE(void* ptr) { Core::Heap.Free(ptr); }

        void VPRINTF(const char* str, va_list args) 
        {
            if (Core::CLI.Enabled) { Debug::PrintFormatted(str, DebugMode::All, args); }
            else { Debug::PrintFormatted(str, DebugMode::Serial, args); }
        }

        void VESA_CLEAR(uint32_t color) { Core::VESA.Clear((Graphics::Color)color); }

        void VESA_BLIT(uint32_t x, uint32_t y, uint32_t color) { Core::VESA.DrawPixel(x, y, (Graphics::Color)color); }

        void VESA_RECTF(uint32_t x, uint32_t y, uint32_t w, uint32_t h, uint32_t color) { Core::VESA.DrawFilledRect(x, y, w, h, (Graphics::Color)color); }

        uint32_t* VESA_GETFB() { return Core::VESA.Buffer; }

        uint32_t  VESA_GETW()  { return Core::VESA.Width; }
        
        uint32_t  VESA_GETH()  { return Core::VESA.Height; }

        bool KB_DOWN(uint8_t key) { return Core::Keyboard.IsKeyDown((Key)key); }
        
        bool KB_UP(uint8_t key) { return Core::Keyboard.IsKeyUp((Key)key); }

        uint32_t MS_GETX() { return Core::Mouse.GetX(); }
        
        uint32_t MS_GETY() { return Core::Mouse.GetY(); }
        
        bool MS_GETLEFT() { return Core::Mouse.IsLeftPressed(); }

        bool MS_GETRIGHT() { return Core::Mouse.IsRightPressed(); }

        DateTime GET_TIME() { return RTC::GetCurrentTime(); }

        bool SEND_MSG(char* name, ProcessMessage msg) { return Core::ProcessMgr.SendMessage(name, msg); }

        ProcessMessage RECV_MSG() { return Core::ProcessMgr.CurrentProc->PopMessage(); }

        bool MSG_READY() { return Core::ProcessMgr.CurrentProc->IsMessageReady(); }

        void YIELD() { Core::ProcessMgr.Schedule(); }
    }

}