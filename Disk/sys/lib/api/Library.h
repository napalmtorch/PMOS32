#pragma once
#include <sys/lib/stdint.h>
#include <sys/lib/stdarg.h>
#include <sys/lib/stddef.h>
#include <sys/lib/api/Time.h>

typedef struct
{
    char     Message[64];
    uint32_t Size;
    uint8_t* Data;
} ProcessMessage;

typedef struct
{
    const char* name;
    uint32_t    addr;
} __attribute__((packed)) mtable_entry_t;

typedef enum
{
    MTYPEID_ERROR,
    MTYPEID_MEM_ALLOCATE,
    MTYPEID_MEM_FREE,
    MTYPEID_VPRINTF,
    MTYPEID_VESA_CLEAR,
    MTYPEID_VESA_BLIT,
    MTYPEID_VESA_RECTF,
    MTYPEID_VESA_GETFB,
    MTYPEID_VESA_GETW,
    MTYPEID_VESA_GETH,
    MTYPEID_KB_DOWN,
    MTYPEID_KB_UP,
    MTYPEID_MS_GETX,
    MTYPEID_MS_GETY,
    MTYPEID_MS_GETLEFT,
    MTYPEID_MS_GETRIGHT,
    MTYPEID_GET_TIME,
} MTYPEID;

typedef void* (*MTYPE_MEM_ALLOCATE)(uint32_t size, bool clear, uint8_t type);
typedef void  (*MTYPE_MEM_FREE)(void* ptr);
typedef void  (*MTYPE_VPRINTF)(const char* str, va_list args);

typedef void  (*MTYPE_VESA_CLEAR)(uint32_t color);
typedef void  (*MTYPE_VESA_BLIT)(uint32_t x, uint32_t y, uint32_t color);
typedef void  (*MTYPE_VESA_RECTF)(uint32_t x, uint32_t y, uint32_t w, uint32_t h, uint32_t color);
typedef void* (*MTYPE_VESA_GETFB)(void);
typedef uint32_t (*MTYPE_VESA_GETW)(void);
typedef uint32_t (*MTYPE_VESA_GETH)(void);

typedef bool (*MTYPE_KB_DOWN)(uint8_t key);
typedef bool (*MTYPE_KB_UP)(uint8_t key);
typedef uint32_t (*MTYPE_MS_GETX)(void);
typedef uint32_t (*MTYPE_MS_GETY)(void);
typedef bool     (*MTYPE_MS_GETLEFT)(void);
typedef bool     (*MTYPE_MS_GETRIGHT)(void);

typedef pmlib::DateTime (*MTYPE_GET_TIME)(void);

typedef bool (*MTYPE_SEND_MSG)(char* proc_name, ProcessMessage msg);
typedef ProcessMessage (*MTYPE_RECV_MSG)(void);
typedef bool (*MTYPE_MSG_READY)(void);

typedef void (*MTYPE_YIELD)(void);

#define MEMTYPE_FREE        0
#define MEMTYPE_USED        1
#define MEMTYPE_ARRAY       2
#define MEMTYPE_PTRARRAY    3
#define MEMTYPE_PROC        4
#define MEMTYPE_THREAD      5
#define MEMTYPE_THREADSTACK 6
#define MEMTYPE_STRING      7
#define MEMTYPE_OBJECT      8
#define MEMTYPE_ERROR       9

extern mtable_entry_t* MTABLE_ENTRIES;

void lib_init();