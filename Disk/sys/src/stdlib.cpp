#include <sys/lib/stdlib.h>
#include <sys/lib/stdio.h>
#include <sys/lib/math.h>
#include <sys/lib/api/Library.h>
#include <sys/lib/api/Memory.h>

EXTC
{
    void* __gxx_personality_v0 = 0;
    void* _Unwind_Resume       = 0;

    const char     PUNCT_CHARS[] = "!\"#$%&'()*+,-./:;<=>?@[\\]^_`{|}~";
    const char*    ITOA_STR = "zyxwvutsrqponmlkjihgfedcba9876543210123456789abcdefghijklmnopqrstuvwxyz";
    const uint32_t LTOA_DIGITS = 32;

    void* malloc(size_t size)
    {
        return kmalloc(size, true, MEMTYPE_USED);
    }

    void free(void* ptr)
    {
        kfree(ptr);
    }

    int atoi(const char* str)
    {
        printf("NOT YET IMPLEMENTED - atoi\n");
        return 0;
    }

    uint32_t atol(const char* str)
    {
        int result = 0, sign = 0;
        uint32_t digit;
        while (*str == 0x20) { str += 1; }

        if (*str == '-') { sign = 1; str += 1; } 
        else 
        { 
            sign = 0;
            if (*str == '+') { str += 1; }
        }

        for (;; str += 1) 
        {
            digit = *str - '0';
            if (digit > 9) { break; }
            result = (10*result) + digit;
        }

        if (sign) { return -result; }
        return result;
    }

    float atof(const char* str)
    {
        printf("NOT YET IMPLEMENTED - atof\n");
        return 0.0f;
    }

    int rand()
    {
        printf("NOT YET IMPLEMENTED - rand\n");
        return 0;
    }

    void clistate(bool state)
    {
        asm volatile("int $0x80": :"a"((uint32_t)state), "b"(0), "c"(0xB0), "d"(0));
        yield();
    }

    void lock() { asm volatile("int $0x80": :"a"(0), "b"(0), "c"(0xF0), "d"(0)); }

    void unlock() { asm volatile("int $0x80": :"a"(0), "b"(0), "c"(0xF1), "d"(0)); }

    void yield() 
    { 
        asm volatile("int $0x80": :"a"(0), "b"(0), "c"(0x6A), "d"(0)); 
        //((MTYPE_YIELD)MTABLE_ENTRIES[20].addr)();
    }

    void exit(int code)
    {
        asm volatile("int $0x80": :"a"(code), "b"(0), "c"(0x69), "d"(0));
        yield();
    }

    void swap(char *x, char *y) { char t = *x; *x = *y; *y = t; }
    
    char* reverse(char *buffer, int i, int j)
    {
        while (i < j) { swap(&buffer[i++], &buffer[j--]); }
        return buffer;
    }
    
    char* itoa(int value, char* buffer, int base)
    {
        if (base < 2 || base > 32) { return buffer; }

        int i = 0, n = abs(value); 
        while (n)
        {
            int r = n % base;
    
            if (r >= 10) { buffer[i++] = 65 + (r - 10); }
            else { buffer[i++] = 48 + r; }
            n = n / base;
        }
    
        if (i == 0) { buffer[i++] = '0'; }
        if (value < 0 && base == 10) { buffer[i++] = '-'; }
        buffer[i] = '\0';
        return reverse(buffer, 0, i - 1);
    }

    void ultoa(unsigned long value, char* result, int base)
    {
        unsigned char index;
        char buffer[LTOA_DIGITS];
        index = LTOA_DIGITS;
        do 
        {
            buffer[--index] = '0' + (value % base);
            if ( buffer[index] > '9') { buffer[index] += 'A' - ':'; }
            value /= base;
        } while (value != 0);

        do { *result++ = buffer[index++]; } while (index < LTOA_DIGITS);
        *result = 0;
    }

    char* ltoa(size_t num, char* str, int base)
    {
        ultoa(num, str, base);
        return str;
    }

    int system(char* command)
    {
        asm volatile("int $0x80": :"a"(0), "b"((uint32_t)command), "c"(0xCC), "d"(0));
        yield();
        return true;
    }
}

void* operator new(size_t size) { return kmalloc(size, true, MEMTYPE_USED); }
void* operator new[](size_t size) { return kmalloc(size, true, MEMTYPE_USED); }
void  operator delete(void *p) { kfree(p); }
void  operator delete(void *p, size_t size) { kfree(p); }
void  operator delete[](void *p) { kfree(p); }
void  operator delete[](void *p, size_t size) { kfree(p); }