#include <Lib/Stream.h>
#include <Kernel/Core/Kernel.h>

// blank stream constructor
Stream::Stream()
{
    Data         = nullptr;
    Size         = 0;
    SeekPosition = 0;
    Access       = (StreamAccess)0;
    Seekable     = false;
}

// stream constructor
Stream::Stream(uint8_t* data, uint32_t size)
{
    Data         = data;
    Size         = size;
    SeekPosition = 0;
    Access       = StreamAccess::ReadWrite;
    Seekable     = true;
}

// stream constructor with specified access
Stream::Stream(uint8_t* data, uint32_t size, StreamAccess access, bool seekable)
{
    Data         = data;
    Size         = size;
    SeekPosition = 0;
    Access       = access;
    Seekable     = seekable;
}

// stream constructor specifying all properties
Stream::Stream(uint8_t* data, uint32_t size, StreamAccess access, bool seekable, uint32_t pos)
{
    Data         = data;
    Size         = size;
    SeekPosition = pos;
    Access       = access;
    Seekable     = seekable;
}

// write uint8_t to current position
void Stream::Write(uint8_t val)
{
    // check if access is granted
    if (Access == StreamAccess::Read) { return; }
    
    // validate seek position
    if (SeekPosition >= Size) { return; }

    // write value to stream
    Data[SeekPosition] = val;
    SeekPosition++;
}

// write specified amount of bytes from buffer to position
void Stream::Write(uint8_t* buffer, uint32_t buffer_size)
{
    // write buffer to stream
    for (uint32_t i = 0; i < buffer_size; i++) 
    { 
        Write(buffer[i]); 
    }
}

// read uint8_t at current position in stream
uint8_t Stream::Read()
{
    // check if access is granted
    if (Access == StreamAccess::Write) { return 0; }

    // validate seek position
    if (SeekPosition >= Size) { return 0; }

    // return value from stream
    return Data[SeekPosition];

}

// read specified amount of bytes from position into buffer
void Stream::Read(uint8_t* buffer, uint32_t buffer_size)
{
    // check if access is granted
    if (Access == StreamAccess::Write) { return; }

    // read data into buffer
    for (uint32_t i = 0; i < buffer_size; i++) { buffer[i] = Read(); }
}

// seek next position
uint8_t Stream::Seek()
{
    // check if seeking is enabled
    if (!Seekable) { return SeekPosition; } 

    // increment and return seek position
    SeekPosition++;
    return SeekPosition;
}

// seek specified position
uint8_t Stream::Seek(uint32_t pos)
{
    // check if seeking is enabled
    if (!Seekable) { return SeekPosition; }

    // set and return seek position
    SeekPosition = pos;
    return SeekPosition;
}

// peek value at current position
uint8_t Stream::Peek()
{
    // check if access is granted
    if (Access == StreamAccess::Write) { return 0; }

    // validate seek position
    if (SeekPosition >= Size) { return 0; }

    // return value
    return Data[SeekPosition];
}

// peek value at specified position
uint8_t Stream::Peek(uint32_t pos)
{
    // check if access is granted
    if (Access == StreamAccess::Write) { return 0; }

    // validate seek position
    if (pos >= Size) { return 0; }

    // return value
    return Data[pos];
}

// get stream data array
uint8_t* Stream::ToArray() { return Data; }

// get stream data size
uint32_t Stream::GetSize() { return Size; }

// get current seeking position
uint32_t Stream::GetPosition() { return SeekPosition; }

// get stream access level
StreamAccess Stream::GetAccess() { return Access; }

// get stream seekability
bool Stream::IsSeekable() { return Seekable; }