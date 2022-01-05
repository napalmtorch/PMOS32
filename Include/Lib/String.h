#pragma once
#include <Lib/Types.h>

char*    strcat(char* dest, const char* src);
char*    strncat(char* dest, const char* src, size_t n);
char*    strchr(const char* str, int c);
char*    strrchr(const char* str, int c);
int      strcmp(const char* str1, const char* str2);
int      strcmpi(const char* str1, const char* str2);
int      strncmp(const char* str1, const char* str2, size_t n);
int      strcoll(const char* str1, const char* str2);
char*    strcpy(char* dest, const char* src);
char*    strncpy(char* dest, const char* src, size_t n);
size_t   strcspn(const char* str1, const char* str2);
size_t   strlen(const char* str);
char*    strpbrk(const char* str1, const char* str2);
size_t   strspn(const char* str1, const char* str2);
char*    strstr(const char* haystack, const char* needle);
char*    strtok(char* str, const char* delim);
size_t   strxfrm(char* dest, const char* src, size_t n);
char*    strdup(const char* str);
char*    strlwr(char* str);
char*    strupr(char* str);
char*    strrev(char* str);
char*    strset(char* str, int c);
char*    strnset(char* str, int c, size_t n);
char*    strhex(uint32_t value, char* result, bool prefix, uint8_t bytes);
bool     streql(const char* str1, const char* str2);
char*    stradd(char* str, int c);
char**   strsplit(char* str, char delim, int* count);
char*    strdel(char* str);