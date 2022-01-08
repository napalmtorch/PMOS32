#pragma once
#include <sys/lib/stdint.h>
#include <sys/lib/stdarg.h>
#include <sys/lib/stddef.h>

void* kmalloc(uint32_t size, bool clear, uint8_t type);
void  kfree(void* ptr);