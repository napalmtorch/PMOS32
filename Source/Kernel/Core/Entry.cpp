#include <Kernel/Core/Kernel.h>

EXTC
{
    // kernel entry point
    void _kernel_entry(void* mboot)
    {
        // start core services
        Kernel::Boot(mboot);
        System::Debug::OK("Finished booting");

        // prepare
        Kernel::BeforeRun();
        System::Debug::Info("Preparing system for interaction");

        // enter main loop
        System::Debug::OK("Entered kernel main loop");
        Core::CLI.PrintCaret();
        while (true) { Kernel::Run(); }        
    }

    // virtual handler
    void __cxa_pure_virtual() { }
}