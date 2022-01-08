#pragma once
#include <Lib/Types.h>
#include <Lib/Memory.h>
#include <Lib/Common.h>
#include <Lib/String.h>
#include <Lib/Math.h>
#include <Kernel/HAL/Common/Multiboot.h>
#include <Kernel/HAL/Common/Ports.h>
#include <Kernel/HAL/Common/Serial.h>
#include <Kernel/HAL/Common/RTC.h>
#include <Kernel/HAL/Common/PIT.h>
#include <Kernel/HAL/Memory/VMM.h>
#include <Kernel/HAL/Memory/PMM.h>
#include <Kernel/HAL/Memory/Heap.h>
#include <Kernel/HAL/Interrupts/GDT.h>
#include <Kernel/HAL/Interrupts/IDT.h>
#include <Kernel/HAL/Drivers/VGA.h>
#include <Kernel/HAL/Drivers/VESA.h>
#include <Kernel/HAL/Drivers/ATA.h>
#include <Kernel/HAL/Drivers/Key.h>
#include <Kernel/HAL/Drivers/PS2Keyboard.h>
#include <Kernel/HAL/Drivers/PS2Mouse.h>
#include <Kernel/System/Debug.h>
#include <Kernel/System/Threading.h>
#include <Kernel/System/Terminal.h>
#include <Kernel/System/Process.h>
#include <Kernel/System/Methods.h>
#include <Kernel/System/SystemCall.h>
#include <Kernel/System/CLI.h>
#include <Kernel/System/ELF.h>
#include <Kernel/FS/DriveManager.h>
#include <Kernel/FS/FileSystem.h>
#include <Kernel/FS/VFS.h>
#include <Kernel/FS/RAMFS.h>

#define KBASE_PHYSICAL 0
#define KBASE_VIRTUAL  0xC0000000

namespace Core
{
    extern HAL::Memory::VirtualMemoryManager VMM;
    extern HAL::Memory::PhysicalMemoryManager PMM;
    extern HAL::Memory::HeapManager Heap;
    extern HAL::Drivers::VGAController VGA;
    extern HAL::Drivers::VESAController VESA;
    extern HAL::Drivers::PS2Keyboard Keyboard;
    extern HAL::Drivers::PS2Mouse Mouse;
    extern HAL::Drivers::ATAController ATA;
    extern System::TerminalManager Terminal;
    extern System::ProcessManager ProcessMgr;
    extern System::CommandLine CLI;
    extern FileSystem::FSHost FileSysHDD;
    extern FileSystem::VirtualFileSystem FS;
    extern FileSystem::RFSHost RAMFS;
}

namespace Kernel
{
    extern HAL::MultibootHeader& Multiboot;

    void Boot(void* mboot);
    void BeforeRun();
    void Run();

    uint32_t GetStartAddress();
    uint32_t GetEndAddress();
    uint32_t GetSize();
}
