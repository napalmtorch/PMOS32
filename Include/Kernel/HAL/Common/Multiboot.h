#pragma once
#include <Lib/Types.h>

namespace HAL
{
    typedef struct
    {
        uint32_t flags;
        uint32_t memory_lower;
        uint32_t memory_upper;
        uint32_t boot_device;
        char*    command_line;
        uint32_t modules_count;
        uint32_t modules_addr;
        uint32_t syms[4];
        uint32_t mmap_len;
        uint32_t mmap_addr;
        uint32_t drives_count;
        uint32_t drives_addr;
        uint32_t config_table;
        char*    bootloader_name;
        uint32_t apm_table;
        uint32_t vbe_ctrl_info;
        uint32_t vbe_mode_info;
        uint32_t vbe_mode;
        uint32_t vbe_interface_seg;
        uint32_t vbe_interface_offset;
        uint32_t vbe_interface_len;
    } PACKED MultibootHeader;
}