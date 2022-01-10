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
        void LOCK(SystemCallArguments args);
        void UNLOCK(SystemCallArguments args);
        void CLISTATE(SystemCallArguments args);
        void EXIT(SystemCallArguments args);
        void YIELD(SystemCallArguments args);
        void CMD(SystemCallArguments args);
    }

    namespace SystemCalls
    {
        static const SystemCall PRINT    = { "PRINT",    0x00000001, SystemCallMethods::PRINT };
        static const SystemCall MTABLE   = { "MTABLE",   0x000000FF, SystemCallMethods::MTABLE }; 
        static const SystemCall EXEC     = { "EXEC",     0x000000A0, SystemCallMethods::EXEC };
        static const SystemCall LOCK     = { "LOCK",     0x000000F0, SystemCallMethods::LOCK };
        static const SystemCall UNLOCK   = { "UNLOCK",   0x000000F1, SystemCallMethods::UNLOCK };
        static const SystemCall CLISTATE = { "CLISTATE", 0x000000B0, SystemCallMethods::CLISTATE };
        static const SystemCall EXIT     = { "EXIT",     0x00000069, SystemCallMethods::EXIT };
        static const SystemCall YIELD    = { "YIELD",    0x0000006A, SystemCallMethods::YIELD };
        static const SystemCall CMD      = { "CMD",      0x000000CC, SystemCallMethods::CMD };
        extern uint32_t Count;

        void Init();
        void Register(SystemCall call);
        void Monitor();
        void Callback(HAL::Registers32* regs);
        void Execute(HAL::Registers32* regs);
        void Execute(SystemCallArguments args);
        bool Push(SystemCallArguments args);
        SystemCallArguments Pop();
    }
}