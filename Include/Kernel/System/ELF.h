#pragma once
#include <Lib/Types.h>
#include <Kernel/System/Process.h>

namespace System
{
    typedef struct
    {
        char    ID[4];
        uint8_t Platform;
        uint8_t Endianness;
        uint8_t Version;
        uint8_t ABI;
        uint8_t ABIVersion;
        uint8_t Reserved[7];
    } PACKED ELFIdentifer;

    typedef struct
    {
        ELFIdentifer Identifier;
        uint16_t     Type;
        uint16_t     Machine;
        uint32_t     Version;
        uint32_t     Entry;
        uint32_t     PHTable;
        uint32_t     SHTable;
        uint32_t     Flags;
        uint16_t     HeaderSize;
        uint16_t     PHEntrySize;  
        uint16_t     PHEntryCount;
        uint16_t     SHEntrySize;
        uint16_t     SHEntryCount;
        uint16_t     SHStringIndex;
    } PACKED ELFHeader;

    typedef struct 
    {
        uint32_t Type;
        uint32_t Offset;
        uint32_t Virtual;
        uint32_t Physical;
        uint32_t FileSize;
        uint32_t SegmentSize;
        uint32_t Flags;
        uint32_t Align;
    } PACKED ELFProgramHeader;

    namespace ELF
    {
        bool Validate(ELFHeader* header);
        bool Init(uint8_t* data, uint32_t size, Process* proc);
    }
}