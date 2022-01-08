#include <sys/lib/api/Memory.h>
#include <sys/lib/api/Library.h>

void* kmalloc(uint32_t size, bool clear, uint8_t type)
{
    return ((MTYPE_MEM_ALLOCATE)MTABLE_ENTRIES[MTYPEID_MEM_ALLOCATE].addr)(size, clear, type);
}

void kfree(void* ptr)
{
    ((MTYPE_MEM_FREE)MTABLE_ENTRIES[MTYPEID_MEM_FREE].addr)(ptr);
}
