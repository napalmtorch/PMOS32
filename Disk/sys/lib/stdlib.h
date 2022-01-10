#pragma once
#include <sys/lib/stdint.h>
#include <sys/lib/stddef.h>

EXTC
{
    void*    malloc(size_t size);
    void     free(void* ptr);
    int      atoi(const char* str);
    uint32_t atol(const char* str);
    float    atof(const char* str);
    int      rand();

    void clistate(bool state);
    void lock();
    void unlock();
    void yield();
    void exit(int code);

    char* itoa(int num, char* str, int base);
    char* ltoa(size_t num, char* str, int base);

    int system(char* command);
}

// allocate overloads
extern void *operator new(size_t size);
extern void *operator new[](size_t size);

// delete overloads
extern void operator delete(void *p);
extern void operator delete(void *p, size_t size);
extern void operator delete[](void *p);
extern void operator delete[](void *p, size_t size);