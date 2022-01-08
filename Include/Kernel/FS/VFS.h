#pragma once
#include <Lib/Types.h>
#include <Kernel/FS/FileSystem.h>

namespace FileSystem
{
    enum class VFSStatus : uint8_t
    {
        Default = 0x00,
        Root    = 0xFF,
    };
    
    typedef struct
    {
        char*     Name;
        char*     Path;
        VFSStatus Status;
        uint32_t  Size;
        uint32_t  SubDirectories;
        uint32_t  SubFiles;
    } PACKED Directory;

    typedef struct
    {
        char*     Name;
        char*     Path;
        VFSStatus Status;
        uint32_t  Size;
        uint8_t*  Data;
    } PACKED File;

    class VirtualFileSystem
    {
        public:
            FSHost* Host;

        public:
            void Init(FSHost* host);

        public:
            bool DirectoryExists(char* path);
            bool FileExists(char* path);

        public:
            Directory GetDirectoryInfo(char* path);
            File      GetFileInfo(char* path);

        public:
            uint32_t GetDirectoryCount(char* path);
            uint32_t GetFileCount(char* path);

        public:
            char** GetDirectories(char* path, uint32_t* count);
            char** GetFiles(char* path, uint32_t* count);

        public:
            char** ReadAllLines(char* path);
            char*  ReadAllText(char* path);
            uint8_t*  ReadAllBytes(char* path, uint32_t* count);

        public:
            bool WriteAllLines(char* path, char** lines, uint32_t count);
            bool WriteAllText(char* path, char* text);
            bool WriteAllBytes(char* path, uint8_t* data, uint32_t size);

        public:
            bool CreateDirectory(char* path);
            bool RenameDirectory(char* path, char* name);
            bool RenameFile(char* path, char* name);
            bool DeleteDirectory(char* path);
            bool DeleteFile(char* path);
            bool CopyDirectory(char* dest, char* src);
            bool CopyFile(char* dest, char* src);
            bool MoveDirectory(char* dest, char* src);
            bool MoveFile(char* dest, char* src);
    };
}