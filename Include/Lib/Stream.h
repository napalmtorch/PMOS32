#pragma once
#include <Lib/Types.h>

static const char C_NEWLINE   = '\n';
static const char C_TERMINATE = '\0';
static const char C_TAB       = '\t';
static const char C_CARRIAGE  = '\r';
static const char C_VERTICAL  = '\v';
static const char C_BLANK     = 0x20;

enum class StreamAccess
{
    ReadWrite,
    Read,
    Write,  
};

class Stream
{
    public:
        uint8_t*     Data;
        uint32_t     Size;
        uint32_t     SeekPosition;
        bool         Seekable;
        StreamAccess Access;

    public:
        Stream();
        Stream(uint8_t* data, uint32_t size);
        Stream(uint8_t* data, uint32_t size, StreamAccess access, bool seekable);
        Stream(uint8_t* data, uint32_t size, StreamAccess access, bool seekable, uint32_t pos);

    public:
        void Write(uint8_t val);
        void Write(uint8_t* buffer, uint32_t buffer_size);
        uint8_t Read();
        void Read(uint8_t* buffer, uint32_t buffer_size);

    public:
        uint8_t Seek();
        uint8_t Seek(uint32_t pos);
        uint8_t Peek();
        uint8_t Peek(uint32_t pos);

    public:
        uint8_t*        ToArray();
        uint32_t         GetSize();
        uint32_t         GetPosition();
        StreamAccess GetAccess();
        bool         IsSeekable();
};