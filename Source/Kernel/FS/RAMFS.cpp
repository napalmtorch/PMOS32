#include <Kernel/FS/RAMFS.h>
#include <Kernel/Core/Kernel.h>
using namespace HAL;
using namespace System;

static const FileSystem::RFSFile NULL_RFSFILE = { { 0 }, 0, 0 };

namespace FileSystem
{
    void RFSHost::Init(uint8_t* data)
    {
        Data = data;
        Info = (RFSInfo*)data;
        Info->Files = (RFSFile*)((uint32_t)Data + sizeof(RFSInfo));
        Debug::OK("Initialized RAMFS - %d files", Info->Count);
    }

    RFSFile RFSHost::ReadFile(const char* name)
    {
        for (uint32_t i = 0; i < Info->CountMax; i++)
        {
            if (Info->Files[i].Size == 0) { continue; }
            if (streql(Info->Files[i].Name, name)) 
            { 
                RFSFile output;
                strcpy(output.Name, Info->Files[i].Name);
                output.Data = (uint32_t)Data + Info->Files[i].Data;
                output.Size = Info->Files[i].Size;
                Debug::Info("Successfully read RAMDISK fie '%s'", name);
                return output;
            }
        }

        Debug::Error("Unable to read RAMDISK file '%s'", name);
        return NULL_RFSFILE;
    }

    RFSFile* RFSHost::GetFiles()
    {
        return Info->Files;
    }
}