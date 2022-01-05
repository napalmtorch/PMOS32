#include <Lib/String.h>
#include <Kernel/Core/Kernel.h>

const char HEX_VALUES[] = "0123456789ABCDEF";

char* strcat(char* dest, const char* src)
{
    if (dest == nullptr) { return nullptr; }
    if (src == nullptr) { return dest; }
    uint64_t src_len = strlen(src);
    uint64_t dest_len = strlen(dest);
    for (uint64_t i = 0; i < src_len; i++) { dest[dest_len + i] = src[i]; }
    dest[strlen(dest)] = 0;
    return dest;
}

char* strncat(char* dest, const char* src, size_t n)
{
    if (dest == nullptr) { return nullptr; }
    if (src == nullptr) { return dest; }
    uint64_t dest_len = strlen(dest);
    for (uint64_t i = 0; i < n; i++) { dest[dest_len + i] = src[i]; }
    dest[strlen(dest)] = 0;
    return dest;
}

char* strchr(const char* str, int c)
{
    
}

char* strrchr(const char* str, int c)
{
    
}

int strcmp(const char* str1, const char* str2)
{
    while (*str1)
    {
        if (*str1 != *str2) { break; }
        str1++; str2++;
    }
    return *(char*)str1 - *(char*)str2;
}

int strcmpi(const char* str1, const char* str2)
{
    
}

int strncmp(const char* str1, const char* str2, size_t n)
{
    while (n && *str1 && (*str1 == *str2))
    {
        ++str1;
        ++str2;
        --n;
    }
    if (n == 0) { return 0; }
    else { return (*(char*)str1 = *(char*)str2); }
    return 0;
}

int strcoll(const char* str1, const char* str2)
{
    
}

char* strcpy(char* dest, const char* src)
{
    memcpy(dest, src, strlen(src));
    return dest;
}

char* strncpy(char* dest, const char* src, size_t n)
{
    for (uint64_t i = 0; i < n; i++) { dest[i] = src[i]; }
    return dest;
}

size_t strcspn(const char* str1, const char* str2)
{
    
}

size_t strlen(const char* str)
{
    uint64_t len = 0;
    while (str[len] != 0) { len++; }
    return len;
}

char* strpbrk(const char* str1, const char* str2)
{
    
}

size_t strspn(const char* str1, const char* str2)
{
    
}

char* strstr(const char* haystack, const char* needle)
{
    
}

char* strtok(char* str, const char* delim)
{
    
}

size_t strxfrm(char* dest, const char* src, size_t n)
{
    
}

char* strdup(const char* str)
{
    
}

char* strlwr(char* str)
{
    for (uint64_t i = 0; i < strlen(str); i++) { str[i] = tolower(str[i]); }
}

char* strupr(char* str)
{
    for (uint64_t i = 0; i < strlen(str); i++) { str[i] = toupper(str[i]); }
}

char* strrev(char* str)
{
	unsigned char a;
	unsigned len = strlen((const char *)str);
	for (int i = 0, j = len - 1; i < j; i++, j--)
	{
		a = str[i];
		str[i] = str[j];
		str[j] = a;
	}
    return str;
}

char* strset(char* str, int c)
{
    for (uint64_t i = 0; i < strlen(str); i++) { str[i] = c; }
    return str;
}

char* strnset(char* str, int c, size_t n)
{
    for (uint64_t i = 0; i < n; i++) { str[i] = c; }
    return str;
}

char* strhex(uint32_t value, char* result, bool prefix, uint8_t bytes)
{
    char* orig = result;
    orig[0] = 0;
    if (prefix) { result[0] = '0'; result[1] = 'x'; result += 2; }
    if (bytes == 1)
    {
        *result = HEX_VALUES[(uint8_t)((value & 0xF0) >> 4)]; result++;
        *result = HEX_VALUES[(uint8_t)((value & 0x0F))];
    }
    else if (bytes == 2)
    {
        *result = HEX_VALUES[(uint8_t)((value & 0xF000) >> 12)]; result++;
        *result = HEX_VALUES[(uint8_t)((value & 0x0F00) >> 8)]; result++;
        *result = HEX_VALUES[(uint8_t)((value & 0x00F0) >> 4)]; result++;
        *result = HEX_VALUES[(uint8_t)((value & 0x000F))]; 
    }
    else if (bytes == 4)
    {
        *result = HEX_VALUES[(uint8_t)((value & 0xF0000000) >> 28)]; result++;
        *result = HEX_VALUES[(uint8_t)((value & 0x0F000000) >> 24)]; result++;
        *result = HEX_VALUES[(uint8_t)((value & 0x00F00000) >> 20)]; result++;
        *result = HEX_VALUES[(uint8_t)((value & 0x000F0000) >> 16)]; result++;
        *result = HEX_VALUES[(uint8_t)((value & 0x0000F000) >> 12)]; result++;
        *result = HEX_VALUES[(uint8_t)((value & 0x00000F00) >> 8)]; result++;
        *result = HEX_VALUES[(uint8_t)((value & 0x000000F0) >> 4)]; result++;
        *result = HEX_VALUES[(uint8_t)((value & 0x0000000F))];
    }
    return orig;
}

bool streql(const char* str1, const char* str2)
{
    if (strcmp(str1, str2) == 0) { return true; }
    return false;
}

char* stradd(char* str, int c)
{
    if (str == nullptr) { return nullptr; }
    uint64_t len = strlen(str);
    str[len] = (char)c;
    str[len + 1] = 0;
    return str;
}

char** strsplit(char* str, char delim, int* count)
{
    if (str == nullptr) { return nullptr; }
    if (strlen(str) == 0) { return nullptr; }

    int len = strlen(str);
    uint32_t num_delimeters = 0;

    for(int i = 0; i < len - 1; i++)
    {
        if(str[i] == delim) { num_delimeters++; }
    }

    uint32_t arr_size = sizeof(char*) * (num_delimeters + 1);
    char** str_array = (char**)Core::Heap.Allocate(arr_size, true, HAL::Memory::HeapType::String);
    int str_offset = 0;

    int start = 0;
    int end = 0;
    while(end < len)
    {
        while(str[end] != delim && end < len) { end++; }

        char* substr = (char*)Core::Heap.Allocate(end - start + 1, true, HAL::Memory::HeapType::String);
        memcpy(substr, str + start, end - start);
        start = end + 1;
        end++;
        str_array[str_offset] = substr;
        str_offset++;
    }

    //return necessary data now
    *count = str_offset;
    return str_array;
}

char* strdel(char* str)
{
    str[strlen(str) - 1] = 0;
    return str;
}