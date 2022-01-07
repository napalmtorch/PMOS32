#pragma once
#include <sys/lib/Types.h>

void* kmalloc(uint32_t size, bool clear, uint8_t type);
void  kfree(void* ptr);

void* memset(void* dest, int c, uint32_t len);
void* memcpy(void* dest, void* src, uint32_t len);