#include <Lib/Common.h>
#include <Kernel/Core/Kernel.h>

const char     PUNCT_CHARS[] = "!\"#$%&'()*+,-./:;<=>?@[\\]^_`{|}~";
const char*    ITOA_STR = "zyxwvutsrqponmlkjihgfedcba9876543210123456789abcdefghijklmnopqrstuvwxyz";
const uint32_t LTOA_DIGITS = 32;

int isalnum(int c)
{ 
    if (c >= 'A' && c <= 'Z') { return true; }
    if (c >= 'a' && c <= 'z') { return true; }
    if (c >= '0' && c <= '9') { return true; }
    return false;
}

int isalpha(int c)
{
    if (c >= 'A' && c <= 'Z') { return true; }
    if (c >= 'a' && c <= 'z') { return true; }
    return false;
}

int iscntrl(int c)
{ 
    if (c >= 0 && c <= 31) { return true; }
    if (c == 127) { return true; }
    return false;
}

int isdigit(int c)
{
    if (c >= '0' && c <= '9') { return true; }
    return false;
}

int isgraph(int c)
{ 
    if (c == ' ') { return false; }
    if (isprint(c)) { return true; }
    return false;
}

int islower(int c)
{ 
    if (c >= 'a' && c <= 'z') { return true; }
    return false;
}

int isprint(int c)
{ 
    if (!iscntrl(c)) { return true; } 
    return false;
}

int ispunct(int c)
{ 
    for (uint32_t i = 0; i < 32; i++) { if (c == PUNCT_CHARS[i]) { return true; } }
    return false;
}

int isspace(int c)
{ 
    if (c == 0x09 || (c >= 0x0A && c <= 0x0D)) { return true; }
    if (c == 0x20) { return true; }
    return false;
}

int isupper(int c)
{ 
    if (c >= 'A' && c <= 'Z') { return true; }
    return false;
}

int isxdigit(int c)
{ 
    if (c >= 'A' && c <= 'F') { return true; }
    if (c >= 'a' && c <= 'f') { return true; }
    if (c >= '0' && c <= '9') { return true; }
    return false;
}

int tolower(int c)
{
    if (c >= 'A' && c <= 'Z') { return c + 32; }
    return c;
}

int toupper(int c)
{ 
    if (c >= 'a' && c <= 'z') { return c - 32; }
    return c;
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

char* dltoa(size_t num, char* str, int base)
{
    return nullptr;
}

uint64_t atodl(const char* str)
{
    return 0;
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

int atoi(const char* str) { return 0; }

uint32_t atol(const char* str)
{
    long int result = 0;
    unsigned int digit;
    int sign;
    while (isspace(*str)) {
	str += 1;
    }

    if (*str == '-') {
	sign = 1;
	str += 1;
    } else {
	sign = 0;
	if (*str == '+') {
	    str += 1;
	}
    }

    for ( ; ; str += 1) {
	digit = *str - '0';
	if (digit > 9) {
	    break;
	}
	result = (10*result) + digit;
    }

    if (sign) {
	return -result;
    }
    return result;
}

void srand(uint32_t seed) { }
int rand(void) { return 0; }
int system(const char* str) { return 0; }
int getchar(void) { return 0; }
char* gets(char* buffer) { return nullptr; }
int printf(const char* format, ...) { return 0; }
int putchar(int c) { return 0; }
int puts(const char* str) { return 0; }