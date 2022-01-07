#pragma once
#include <sys/lib/Types.h>
#include <sys/lib/Library.h>

void kprint(const char* str);
void vprintf(const char* str, va_list args);
void printf(const char* str, ...);

void clistate(bool state);
void lock();
void unlock();
void exit(int code);

static inline int abs(int i) { return i < 0 ? -i : i; }

char* itoa(int num, char* str, int base);
char* ltoa(size_t num, char* str, int base);