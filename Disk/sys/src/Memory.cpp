#include <sys/lib/Memory.h>
#include <sys/lib/Library.h>

void* kmalloc(uint32_t size, bool clear, uint8_t type)
{
    return ((MTYPE_MEM_ALLOCATE)MTABLE_ENTRIES[MTYPEID_MEM_ALLOCATE].addr)(size, clear, type);
}

void kfree(void* ptr)
{
    ((MTYPE_MEM_FREE)MTABLE_ENTRIES[MTYPEID_MEM_FREE].addr)(ptr);
}

void* memset(void* dest, int c, uint32_t len)
{
    len /= 4;
    asm volatile ("cld; rep stosl" : "+c" (len), "+D" (dest) : "a" (c) : "memory");
    return dest;
}

void* memcpy(void* dest, void* src, uint32_t len)
{
    uint32_t  destt      = (uint32_t)dest;
    uint32_t  srcc       = (uint32_t)src;
    uint32_t  num_dwords = len / 4;
    uint32_t  num_bytes  = len % 4;
    uint32_t* dest32     = (uint32_t*)destt;
    uint32_t* src32      = (uint32_t*)srcc;
    uint8_t*  dest8      = ((uint8_t*)destt) + num_dwords * 4;
    uint8_t*  src8       = ((uint8_t*)srcc) + num_dwords * 4;
    uint32_t  i;

    for (i = 0; i < num_dwords; i++) { dest32[i] = src32[i]; }
    for (i = 0; i < num_bytes; i++)  { dest8[i]  = src8[i]; }
    return dest;
}