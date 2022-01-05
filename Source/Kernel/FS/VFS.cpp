#include <Kernel/FS/VFS.h>
#include <Kernel/Core/Kernel.h>
using namespace HAL;
using namespace System;
using namespace Graphics;

FileSystem::Directory VFS_NULL_DIR = { 0 };
FileSystem::File      VFS_NULL_FILE = { 0 };

namespace FileSystem
{
    void VirtualFileSystem::Init(FSHost* host)
    {
        Host = host;
        Debug::OK("Initialized virtual file system");
    }

    bool VirtualFileSystem::DirectoryExists(char* path)
    {
        FSDirectory dir = Host->GetDirectoryByName(path);
        if (dir.Type != FSTYPE_DIR) { return false; }
        return true;
    }

    bool VirtualFileSystem::FileExists(char* path)
    {
        FSFile file = Host->GetFileByName(path);
        if (file.Type != FSTYPE_FILE) { return false; }
        return true;
    }

    Directory VirtualFileSystem::GetDirectoryInfo(char* path)
    {
        FSDirectory dir = Host->GetDirectoryByName(path);
        if (dir.Type != FSTYPE_DIR) { return VFS_NULL_DIR; }
        char* name = Host->GetNameFromPath(path);
        char* dir_name = Host->GetParentPathFromPath(path);
        if (name == NULL) { return VFS_NULL_DIR; }

        Directory out_dir;
        out_dir.Name = (char*)Core::Heap.Allocate(strlen(name) + 1, true, Memory::HeapType::String);
        strcpy(out_dir.Name, name);
        
        // copy name and path
        if (dir_name != NULL)
        {
            out_dir.Path = (char*)Core::Heap.Allocate(strlen(dir_name) + 1, true, Memory::HeapType::String);
            strcpy(out_dir.Path, dir_name);
        }
        else { out_dir.Path = (char*)Core::Heap.Allocate(1, true, Memory::HeapType::String); out_dir.Path[0] = 0; }

        out_dir.Status = (VFSStatus)dir.Status;

        // not yet implemented
        out_dir.Size = 0;
        out_dir.SubDirectories = 0;
        out_dir.SubFiles = 0;

        return out_dir;
    }

    File VirtualFileSystem::GetFileInfo(char* path)
    {
        FSFile file = Host->GetFileByName(path);
        if (file.Type != FSTYPE_FILE) { return VFS_NULL_FILE; }
        char* name = Host->GetNameFromPath(path);
        char* dir_name = Host->GetParentPathFromPath(path);
        if (name == NULL) { return VFS_NULL_FILE; }

        File out_file;
        out_file.Name = (char*)Core::Heap.Allocate(strlen(name) + 1, true, Memory::HeapType::String);
        strcpy(out_file.Name, name);
        Core::Heap.Free(name);
        
        // copy name and path
        if (dir_name != NULL)
        {
            out_file.Path = (char*)Core::Heap.Allocate(strlen(dir_name) + 1, true, Memory::HeapType::String);
            strcpy(out_file.Path, dir_name);
        }
        else { out_file.Path = (char*)Core::Heap.Allocate(1, true, Memory::HeapType::String); out_file.Path[0] = 0; }

        Core::Heap.Free(dir_name);
        out_file.Status = (VFSStatus)file.Status;
        out_file.Size = file.Size;
        out_file.Data = nullptr;
        return out_file;
    }

    uint32_t VirtualFileSystem::GetDirectoryCount(char* path)
    {
        FSDirectory dir = Host->GetDirectoryByName(path);
        if (dir.Type != FSTYPE_DIR) { Debug::Error("Error while counting directories in '%s'", path); return 0; }
        int index = Host->GetDirectoryIndex(dir);

        uint32_t dirs_count = 0;
        uint8_t* secdata = (uint8_t*)Core::Heap.Allocate(Core::ATA.SectorSize, true, Memory::HeapType::Array);
        for (uint32_t sec = 0; sec < Host->Info.FileTableMaxUsed; sec++)
        {
            Core::ATA.Read(Host->Info.FileTableStart + sec, 1, secdata);

            for (uint32_t i = 0; i < Core::ATA.SectorSize; i += sizeof(FSDirectory))
            {
                FSDirectory* tempdir = (FSDirectory*)(secdata + i);
                if (tempdir->Type == FSTYPE_DIR && tempdir->ParentIndex == index) 
                {
                    dirs_count++; 
                }
            }
        }
        
        Core::Heap.Free(secdata);
        return dirs_count;
    }

    uint32_t VirtualFileSystem::GetFileCount(char* path)
    {
        if (!DirectoryExists(path)) { Debug::Error("Unable to count files in '%s'", path); return 0; }

        FSDirectory dir = Host->GetDirectoryByName(path);
        if (dir.Type != FSTYPE_DIR) { Debug::Error("Error while counting files in '%s'", path); return 0; }
        int index = Host->GetDirectoryIndex(dir);

        uint32_t dirs_count = 0;
        uint8_t* secdata = (uint8_t*)Core::Heap.Allocate(Core::ATA.SectorSize, true, Memory::HeapType::Array);
        for (uint32_t sec = 0; sec < Host->Info.FileTableMaxUsed; sec++)
        {
            Core::ATA.Read(Host->Info.FileTableStart + sec, 1, secdata);

            for (uint32_t i = 0; i < Core::ATA.SectorSize; i += sizeof(FSFile))
            {
                FSFile* tempfile = (FSFile*)(secdata + i);
                if (tempfile->Type == FSTYPE_FILE && tempfile->ParentIndex == index) 
                { 
                    dirs_count++; 
                }
            }
        }
        Core::Heap.Free(secdata);
        return dirs_count;
    }

    char** VirtualFileSystem::GetDirectories(char* path, uint32_t* count)
    {
        if (!DirectoryExists(path)) { Debug::Error("Unable to locate directory '%s'", path); return nullptr; }

        FSDirectory dir = Host->GetDirectoryByName(path);
        if (dir.Type != FSTYPE_DIR) { Debug::Error("Error while locating directory '%s'", path); return nullptr; }
        int index = Host->GetDirectoryIndex(dir);

        uint32_t dir_count = GetDirectoryCount(path);
        if (dir_count == 0) { *count = 0; return nullptr; }
        char** output = (char**)Core::Heap.Allocate(sizeof(char*) * dir_count, true, Memory::HeapType::PointerArray);
        int output_index = 0;

        uint8_t* secdata = (uint8_t*)Core::Heap.Allocate(Core::ATA.SectorSize, true, Memory::HeapType::Array);
        for (uint32_t sec = 0; sec < Host->Info.FileTableMaxUsed; sec++)
        {
            Core::ATA.Read(Host->Info.FileTableStart + sec, 1, secdata);

            for (uint32_t i = 0; i < Core::ATA.SectorSize; i += sizeof(FSDirectory))
            {
                FSDirectory* tempdir = (FSDirectory*)(secdata + i);
                if (tempdir->Type == FSTYPE_DIR && tempdir->ParentIndex == index)
                {
                    char* dirname = (char*)Core::Heap.Allocate(strlen(tempdir->Name) + 1, true, Memory::HeapType::String);
                    strcpy(dirname, tempdir->Name);
                    output[output_index] = dirname;
                    output_index++;
                }
            }
        }

        Core::Heap.Free(secdata);
        *count = output_index;
        return output;
    }

    char** VirtualFileSystem::GetFiles(char* path, uint32_t* count)
    {
        if (!DirectoryExists(path)) { Debug::Error("Unable to locate directory '%s'\n", path); return NULL; }

        FSDirectory dir = Host->GetDirectoryByName(path);
        if (dir.Type != FSTYPE_DIR) { Debug::Error("Error while locating directory '%s'\n", path); return NULL; }
        int index = Host->GetDirectoryIndex(dir);

        uint32_t file_count = GetFileCount(path);
        if (file_count == 0) { *count = 0; return NULL; }
        char** output = (char**)Core::Heap.Allocate(sizeof(char*) * file_count, true, Memory::HeapType::PointerArray);
        int output_index = 0;

        uint8_t* secdata = (uint8_t*)Core::Heap.Allocate(Core::ATA.SectorSize, true, Memory::HeapType::Array);
        for (uint32_t sec = 0; sec < Host->Info.FileTableMaxUsed; sec++)
        {
            Core::ATA.Read(Host->Info.FileTableStart + sec, 1, secdata);

            for (uint32_t i = 0; i < Core::ATA.SectorSize; i += sizeof(FSFile))
            {
                FSFile* tempfile = (FSFile*)(secdata + i);
                if (tempfile->Type == FSTYPE_FILE && tempfile->ParentIndex == index)
                {
                    char* fname = (char*)Core::Heap.Allocate(strlen(tempfile->Name) + 1, true, Memory::HeapType::String);
                    strcpy(fname, tempfile->Name);
                    output[output_index] = fname;
                    output_index++;
                }
            }
        }
        Core::Heap.Free(secdata);
        *count = output_index;
        return output;
    }

    char** VirtualFileSystem::ReadAllLines(char* path)
    {

    }
    
    char* VirtualFileSystem::ReadAllText(char* path)
    {
        FSFile file = Host->ReadFile(path);
        if (file.Type != FSTYPE_FILE) { return NULL; }
        return (char*)file.Data;
    }

    uint8_t* VirtualFileSystem::ReadAllBytes(char* path, uint32_t* count)
    {
        FSFile file = Host->ReadFile(path);
        if (file.Type != FSTYPE_FILE) { return NULL; }
        *count = file.Size;
        return file.Data;
    }

    bool VirtualFileSystem::WriteAllLines(char* path, char** lines, uint32_t count)
    {

    }

    bool VirtualFileSystem::WriteAllText(char* path, char* text)
    {
        return Host->WriteFile(path, (uint8_t*)text, strlen(text) + 1);
    }

    bool VirtualFileSystem::WriteAllBytes(char* path, uint8_t* data, uint32_t size)
    {
        return Host->WriteFile(path, data, size);
    }

    bool VirtualFileSystem::CreateDirectory(char* path)
    {
        FSDirectory parent = Host->GetParentFromPath(path);
        if (parent.Type != FSTYPE_DIR) { return false; }

        FSDirectory new_dir;
        char* name = Host->GetNameFromPath(path);
        strcpy(new_dir.Name, name);
        Core::Heap.Free(name);
        new_dir.ParentIndex = Host->GetDirectoryIndex(parent);
        new_dir.Status = 0x00;
        new_dir.Type   = FSTYPE_DIR;
        memset(new_dir.Padding, 0, sizeof(new_dir.Padding));
        FSDirectory created = Host->FileEntries.CreateDirectory(new_dir);
        if (created.Type != FSTYPE_DIR) { return false; }
        return true;
    }

    bool VirtualFileSystem::RenameDirectory(char* path, char* name)
    {
        FSDirectory dir = Host->GetDirectoryByName(path);
        if (dir.Type != FSTYPE_DIR) { return false; }
        int index = Host->GetDirectoryIndex(dir);

        strcpy(dir.Name, name);
        Host->FileEntries.WriteDirectory(index, dir);
        return true;
    }

    bool VirtualFileSystem::RenameFile(char* path, char* name)
    {
        FSFile file = Host->GetFileByName(path);
        if (file.Type != FSTYPE_FILE) { return false; }
        int index = Host->GetFileIndex(file);

        strcpy(file.Name, name);
        Host->FileEntries.WriteFile(index, file);
        return true;
    }

    bool VirtualFileSystem::DeleteDirectory(char* path)
    {
        return false;
    }

    bool VirtualFileSystem::DeleteFile(char* path)
    {
        FSFile file = Host->GetFileByName(path);
        if (file.Type != FSTYPE_FILE) { return false; }

        if (!Host->BlockEntries.Free(Host->BlockEntries.Get(file.BlockIndex))) { return false; }
        return Host->FileEntries.DeleteFile(file);
    }

    bool VirtualFileSystem::CopyDirectory(char* dest, char* src)
    {
        return false;
    }

    bool VirtualFileSystem::CopyFile(char* dest, char* src)
    {
        FSFile file_src = Host->ReadFile(src);
        if (file_src.Type != FSTYPE_FILE) { return false; }

        FSDirectory file_dest_parent = Host->GetParentFromPath(dest);
        if (file_dest_parent.Type != FSTYPE_DIR) { return false; }

        char* name = Host->GetNameFromPath(dest);
        FSFile file_dest;
        strcpy(file_dest.Name, name);
        file_dest.ParentIndex = Host->GetDirectoryIndex(file_dest_parent);
        file_dest.Status = 0x00;
        file_dest.Type = FSTYPE_FILE;
        file_dest.Size = file_src.Size;
        
        FSBlockEntry new_blk = Host->BlockEntries.Allocate(Host->GetSectorsFromBytes(file_dest.Size));
        Host->BlockEntries.Copy(new_blk, Host->BlockEntries.Get(file_src.BlockIndex));

        file_dest.BlockIndex = Host->BlockEntries.GetIndex(new_blk);
        return Host->FileEntries.CreateFile(file_dest).Type == FSTYPE_FILE;
    }

    bool VirtualFileSystem::MoveDirectory(char* dest, char* src)
    {
        return false;
    }

    bool VirtualFileSystem::MoveFile(char* dest, char* src)
    {
        return false;
    }
}