#include <Kernel/Core/Kernel.h>
using namespace HAL;
using namespace System;
using namespace Graphics;
using namespace FileSystem;

EXTC
{
    extern uint32_t _kernel_start;
    extern uint32_t _kernel_end;
}

namespace Core
{
    HAL::Memory::VirtualMemoryManager VMM;
    HAL::Memory::PhysicalMemoryManager PMM;
    HAL::Memory::HeapManager Heap;
    HAL::Drivers::VGAController VGA;
    HAL::Drivers::VESAController VESA;
    HAL::Drivers::PS2Keyboard Keyboard;
    HAL::Drivers::PS2Mouse Mouse;
    HAL::Drivers::ATAController ATA;
    System::TerminalManager Terminal;
    System::ProcessManager ProcessMgr;
    System::CommandLine CLI;
    FileSystem::FSHost FileSysHDD;
    FileSystem::VirtualFileSystem FS;
    FileSystem::RFSHost RAMFS;
}

namespace Kernel
{
    // Copy of multiboot header
    MultibootHeader& Multiboot;

    /// Entry point for idle thread
    int IdleMain(void* arg)
    {
        lock(); 
        Debug::Info("Entered idle thread"); 
        unlock();

        while (true) { Core::ProcessMgr.Schedule(); }
        return 0;
    }

    // pit callback
    int PITTime;
    void PITCallback(Registers32* regs)
    {
        PITTime++;
        if (PITTime >= 1000) { PITTime = 0; }
    }

    static FileSystem::RFSFile test_program;

    // Initialize and start core system services
    void Boot(void* mboot)
    {
        // initialize serial
        Serial::SetPort(SerialPort::COM1);
        Debug::SetMode(DebugMode::Serial);
        Debug::Info("Starting PurpleMoon...");

        // copy multiboot
        MultibootHeader* mboot_hdr = (MultibootHeader*)mboot;
        Multiboot = *mboot_hdr;
        Debug::OK("Located multiboot header");
        Debug::Info("Bootloader: %s", Multiboot.bootloader_name);
        Debug::Info("Kernel Memory: 0x%8x - 0x%8x", GetStartAddress(), GetEndAddress());

        // initialize descriptors
        GDT::Init();
        IDT::Init();
        asm volatile("cli");

        // initialize memory management
        Core::VMM.Init(0x0A000000);
        Core::PMM.Init();
        Core::Heap.Init(128 * 1024 * 1024, 4096);

        Core::PMM.PrintTable(&Core::PMM.UsedTable);

        DriveManager::Init();

        // initialize ram disk
        MultibootModule* mod = (MultibootModule*)Multiboot.modules_addr;
        uint8_t* ramdisk = (uint8_t*)mod->StartAddress;
        Debug::Info("RAM DISK ADDR: 0x%8x - 0x%8x", mod->StartAddress, mod->EndAddress);
        Core::RAMFS.Init((uint8_t*)(KBASE_VIRTUAL + (uint32_t)ramdisk));

        // initialize vga
        Core::VESA.Init();

        // initialize terminal
        Core::Terminal.Init(TerminalType::VESA);
        Core::Terminal.WriteLine("Starting PurpleMoon...");

        // initialize ata driver
        Core::ATA.Init();

        // initialize file system
        if (Core::ATA.Initialized)
        {
            Core::FileSysHDD.Mount();
            Core::FS.Init(&Core::FileSysHDD);
        }

        // initialize process manager
        Core::ProcessMgr.Init();

        MethodLibrary::Init();

        // create idle thread and add to kernel process
        Threading::Thread* idle = Threading::Thread::Create("idle", 16384, IdleMain);
        idle->Registers.CR3 = Core::VMM.KernelDirectory.GetPhysical();
        Core::ProcessMgr.Processes[0]->LoadThread(idle);
        
        test_program = Core::RAMFS.ReadFile("shell.elf");
        Process* test_proc = Process::CreateELF("Shell.elf", test_program.Data, test_program.Size);
        Core::ProcessMgr.Load(test_proc);

        // initialize PS2 keyboard
        Core::Keyboard.Init();
        Core::Mouse.Init();

        // initialize command line
        Core::CLI.Init();

        // initialize PIT
        PIT::Init(1000, PITCallback);

        RTC::Init();

        SystemCalls::Init();

        // enable interrupts
        asm volatile("sti");
    }

    // Prepare system for user interaction
    void BeforeRun()
    {

    }

    // Kernel main loop
    void Run()
    {
        Core::CLI.Monitor();
        SystemCalls::Monitor();
        Core::ProcessMgr.Schedule();
    }

    /// @brief Get physical start address of kernel memory @return Physical start address
    uint32_t GetStartAddress() { return (uint32_t)&_kernel_start; }

    /// @brief Get physical end address of kernel memory @return Physical end address
    uint32_t GetEndAddress() { return (uint32_t)&_kernel_end - KBASE_VIRTUAL; }

    /// @brief Get size of kernel memory @return Size
    uint32_t GetSize() { return GetEndAddress() - GetStartAddress(); }
}