#pragma once
#include <Lib/Types.h>
#include <Kernel/System/Debug.h>

#define FS_SECTOR_BOOT  0
#define FS_SECTOR_INFO  1
#define FS_SECTOR_BLKS  4

#define FSSTATE_FREE 0
#define FSSTATE_USED 1

#define FSTYPE_NULL 0
#define FSTYPE_DIR  1
#define FSTYPE_FILE 2

namespace FileSystem
{
    typedef struct
    {
        uint32_t SectorCount;
        uint16_t BytesPerSector; 
        uint32_t BlockTableStart;
        uint32_t BlockTableCount;
        uint32_t BlockTableCountMax;
        uint32_t BlockTableSectorCount;
        uint32_t BlockDataStart;
        uint32_t BlockDataSectorCount;
        uint32_t BlockDataUsed;
        uint32_t FileTableStart;
        uint32_t FileTableCount;
        uint32_t FileTableCountMax;
        uint32_t FileTableSectorCount;
        uint32_t FileTableMaxUsed;
    } PACKED FSInfo;

    typedef struct
    {
        uint32_t Start;
        uint32_t Count;
        uint16_t State;
        uint8_t  Padding[6];
    } PACKED FSBlockEntry;

    typedef struct
    {
        char     Name[46];
        uint32_t ParentIndex;
        uint8_t  Status;
        uint8_t  Type;
        uint8_t  Padding[12];
    } PACKED FSDirectory;

    typedef struct
    {
        char     Name[46];
        uint32_t ParentIndex;
        uint8_t  Status;
        uint8_t  Type;
        uint32_t Size;
        uint32_t BlockIndex;
        uint8_t* Data;
    } PACKED FSFile;

    static const FSBlockEntry NULL_FSBLOCKENTRY = { 0, 0, 0, { 0, 0, 0, 0, 0, 0 } };
    static const FSDirectory  NULL_FSDIRECTORY  = { "", 0, 0, 0, { 0 } };
    static const FSFile       NULL_FILE         = { "", 0, 0, 0, 0, 0, nullptr };

    class FSHost;
    class FSBlockTable;
    class FSFileTable;

    class FSBlockTable
    {
        public:
            FSHost*      Host;
            FSBlockEntry Mass;
            FSBlockEntry Files;

        public:
            void     Init(FSHost* host);
            void     Print(System::DebugMode mode);

        public:
            FSBlockEntry Read(int index);
            bool         Write(int index, FSBlockEntry entry);
            bool         Copy(FSBlockEntry dest, FSBlockEntry src);
            bool         Fill(FSBlockEntry entry, uint8_t value);

        public:
            FSBlockEntry Allocate(uint32_t sectors);
            bool         Free(FSBlockEntry entry);
            void         MergeFreeBlocks();
            FSBlockEntry GetNearest(FSBlockEntry entry);
            FSBlockEntry CreateEntry(uint32_t start, uint32_t count, uint8_t state);
            bool         DeleteEntry(FSBlockEntry entry);

        public:
            FSBlockEntry Get(int index);
            bool         ValidateSector(uint32_t sector);
            int          GetIndex(FSBlockEntry entry);
            int          GetFreeIndex();
            uint32_t     GetSectorFromIndex(int index);
            uint32_t     GetOffsetFromIndex(uint32_t sector, int index);
    };

    class FSFileTable
    {
        public:
            FSHost* Host;
            
        public:
            void Init(FSHost* host);
            void Print(System::DebugMode mode);

        public:
            FSDirectory ReadDirectory(int index);
            FSFile      ReadFile(int index);
            bool        WriteDirectory(int index, FSDirectory dir);
            bool        WriteFile(int index, FSFile file);
            FSDirectory CreateDirectory(FSDirectory dir);
            FSFile      CreateFile(FSFile file);
            bool        DeleteDirectory(FSDirectory dir);
            bool        DeleteFile(FSFile file);

        public:
            bool     ValidateSector(uint32_t sector);
            int      GetDirectoryIndex(FSDirectory dir);
            int      GetFileIndex(FSFile file);
            int      GetFreeIndex();
            uint32_t GetSectorFromIndex(int index);
            uint32_t GetOffsetFromIndex(uint32_t sector, int index);
    };  

    class FSHost
    {
        public:
            FSInfo       Info;
            FSBlockTable BlockEntries;
            FSFileTable  FileEntries;
            FSDirectory  RootDir;

        public:
            void Mount();
            void Unmount();
            void Format(uint32_t size, bool wipe);
            void Wipe(uint32_t size);
            void CreateRoot(char* label);

        public:
            void CreateInfo(uint32_t size);
            void ReadInfo();
            void WriteInfo();
            FSInfo GetInfo();

        public:
            bool        DirectoryEquals(FSDirectory a, FSDirectory b);
            bool        FileEquals(FSFile a, FSFile b);
            FSDirectory GetParentFromPath(char* path);
            FSDirectory GetDirectoryByName(char* path);
            FSFile      GetFileByName(char* path);
            int         GetDirectoryIndex(FSDirectory dir);
            int         GetFileIndex(FSFile file);
            char*       GetNameFromPath(char* path);
            char*       GetParentPathFromPath(char* path);
            FSFile      CreateFile(char* path, uint32_t size);
            FSFile      ReadFile(char* path);
            bool        WriteFile(char* path, uint8_t* data, uint32_t len);

        public:
            uint32_t GetSectorsFromBytes(uint32_t bytes);
    };
}