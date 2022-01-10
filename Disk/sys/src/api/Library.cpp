#include <sys/lib/api/Library.h>

mtable_entry_t* MTABLE_ENTRIES;

void lib_init()
{
    asm volatile("int $0x80": :"a"(0), "b"(0), "c"(0xFF), "d"((unsigned int)&MTABLE_ENTRIES));
}

    // virtual handler
EXTC void __cxa_pure_virtual() { }