#pragma once
#include <Lib/Types.h>
#include <Kernel/HAL/Interrupts/IDT.h>

namespace System
{
    typedef struct
    {
        uint32_t Argument;                  // EAX
        void*    Source;                    // EBX
        uint32_t Code;                      // ECX
        void*    Destination;               // EDX
    } PACKED SystemCallArguments;

    typedef void (*SystemCallHandler)(SystemCallArguments args);

    typedef struct
    {
        const char*       Name;
        uint32_t          Code;
        SystemCallHandler Handler;
    } PACKED SystemCall;

    namespace SystemCallMethods
    {
        void PRINT(SystemCallArguments args);
        void MTABLE(SystemCallArguments args);
        void EXEC(SystemCallArguments args);
    }

    namespace SystemCalls
    {
        static const SystemCall PRINT  = { "PRINT",  0x00000001, SystemCallMethods::PRINT };
        static const SystemCall MTABLE = { "MTABLE", 0x000000FF, SystemCallMethods::MTABLE }; 
        static const SystemCall EXEC   = { "EXEC",   0x000000A0, SystemCallMethods::EXEC };

        extern uint32_t Count;

        void Init();
        void Register(SystemCall call);
        void Execute(HAL::Registers32* regs);
        void Execute(SystemCallArguments args);
    }
}