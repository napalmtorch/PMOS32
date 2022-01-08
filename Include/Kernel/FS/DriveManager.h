#pragma once
#include <Lib/Types.h>

namespace FileSystem
{
    enum class DriveType : uint8_t
    {
        Invalid,
        RAMDisk,
        ATADisk,
        ATAOptical,
        SATADisk,
        SATAOptical,
    };

    enum class FileSystemType : uint8_t
    {
        Invalid,
        PMRFS,
        PMFS,
        FAT,
        EXT2,
    };

    typedef struct
    {
        char           Label[32];
        char           Letter;
        DriveType      Type;
        FileSystemType FSType;
    } PACKED VirtualDrive;

    namespace DriveManager
    {
        extern VirtualDrive* Drives[];

        void Init();
        bool Register(VirtualDrive* drive);
        VirtualDrive* Register(DriveType type, FileSystemType fs_type, char letter, char* label);
        bool Unregister(VirtualDrive* drive, bool free = true);
        bool Unregister(char letter, bool free = true);
        bool Unregister(char* label, bool free = true);
    }
}