#include <sys/lib/Common.h>

const char     PUNCT_CHARS[] = "!\"#$%&'()*+,-./:;<=>?@[\\]^_`{|}~";
const char*    ITOA_STR = "zyxwvutsrqponmlkjihgfedcba9876543210123456789abcdefghijklmnopqrstuvwxyz";
const uint32_t LTOA_DIGITS = 32;

void kprint(const char* str)
{
    asm volatile("int $0x80": :"a"(0), "b"((unsigned int)str), "c"(0x01), "d"(0));
}

void vprintf(const char* str, va_list args)
{
    ((MTYPE_VPRINTF)MTABLE_ENTRIES[MTYPEID_VPRINTF].addr)(str, args);
}

void printf(const char* str, ...)
{
    va_list args;
    va_start(args, str);
    vprintf(str, args);
    va_end(args);
}

void clistate(bool state)
{
    asm volatile("int $0x80": :"a"((uint32_t)state), "b"(0), "c"(0xB0), "d"(0));
}

void lock() { asm volatile("int $0x80": :"a"(0), "b"(0), "c"(0xF0), "d"(0)); }

void unlock() { asm volatile("int $0x80": :"a"(0), "b"(0), "c"(0xF1), "d"(0)); }

void exit(int code)
{
    asm volatile("int $0x80": :"a"(code), "b"(0), "c"(0x69), "d"(0));
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
    if (num < 0 && base == 10) 
    {
        *str++ = '-';
        ultoa(-num, str, base);
        return str;
    }
    ultoa(num, str, base);
    return str;
}

void strrevl(char* str, int len)
{
    int i = 0, j = len - 1, temp;
    while (i < j) 
    {
        temp = str[i];
        str[i] = str[j];
        str[j] = temp;
        i++;
        j--;
    }
}

int int2str(int x, char str[], int d)
{
    int i = 0;
    while (x) { str[i++] = (x % 10) + '0'; x = x / 10; }
    while (i < d) { str[i++] = '0'; }
    strrevl(str, i);
    str[i] = '\0';
    return i;
}

void swap(char *x, char *y) {
    char t = *x; *x = *y; *y = t;
}
 
char* reverse(char *buffer, int i, int j)
{
    while (i < j) {
        swap(&buffer[i++], &buffer[j--]);
    }
 
    return buffer;
}
 
char* itoa(int value, char* buffer, int base)
{
    // invalid input
    if (base < 2 || base > 32) {
        return buffer;
    }

    int n = abs(value);
 
    int i = 0;
    while (n)
    {
        int r = n % base;
 
        if (r >= 10) {
            buffer[i++] = 65 + (r - 10);
        }
        else {
            buffer[i++] = 48 + r;
        }
 
        n = n / base;
    }
 
    if (i == 0) {
        buffer[i++] = '0';
    }

    if (value < 0 && base == 10) {
        buffer[i++] = '-';
    }
 
    buffer[i] = '\0';

    return reverse(buffer, 0, i - 1);
}