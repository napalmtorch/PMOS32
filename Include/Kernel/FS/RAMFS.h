#pragma once
#include <Lib/Types.h>
#include <Kernel/FS/DriveManager.h>

namespace FileSystem
{
    typedef struct
    {
        char     Name[32];
        uint8_t* Data;
        uint32_t Size;
    } PACKED RFSFile;

    typedef struct
    {
        uint32_t Count;
        uint32_t CountMax;
        uint32_t CurrentAddress;
        RFSFile* Files;
    } PACKED RFSInfo;

    class RFSHost
    {
        private:
            uint8_t* Data;
            RFSInfo* Info;
            VirtualDrive* Drive;

        public:
            void Init(uint8_t* data);
            RFSFile ReadFile(const char* name);
            RFSFile* GetFiles();
    };
}