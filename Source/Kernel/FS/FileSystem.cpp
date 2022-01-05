#include <Kernel/FS/FileSystem.h>
#include <Kernel/Core/Kernel.h>
using namespace HAL;
using namespace System;
using namespace Graphics;

// null structures
FileSystem::FSBlockEntry NULL_BLKENTRY = { 0, 0, 0, { 0, 0, 0, 0, 0, 0 } };
FileSystem::FSDirectory  NULL_DIR      = { "", 0, 0, 0, { 0 } };
FileSystem::FSFile       NULL_FILE     = { "", 0, 0, 0, 0, 0, NULL };

namespace FileSystem
{
    void FSBlockTable::Init(FSHost* host)
    {
        Host = host;

        Mass  = Read(0);
        Files = Read(1);
    }

    void FSBlockTable::Print(DebugMode mode)
    {
        Host->ReadInfo();
        uint8_t* data = (uint8_t*)Core::Heap.Allocate(Core::ATA.SectorSize, true, Memory::HeapType::Array);

        int index = 0;
        for (uint32_t sec = 0; sec < Host->Info.BlockTableSectorCount; sec++)
        {
            Core::ATA.Read(Host->Info.BlockTableStart + sec, 1, data);

            for (uint32_t i = 0; i < Core::ATA.SectorSize; i += sizeof(FSBlockEntry))
            {
                FSBlockEntry* entry = (FSBlockEntry*)(data + i);
                if (entry->Start == 0) { index++; continue; }
                Debug::PrintFormatted("INDEX: 0x%8x START: 0x%8x COUNT: 0x%8x STATE: 0x%2x\n", mode, index, entry->Start, entry->Count, entry->State);
                index++;
            }
        }

        Debug::PrintFormatted("\n");
        Core::Heap.Free(data);
    }

    FSBlockEntry FSBlockTable::Read(int index)
    {
        Host->ReadInfo();
        if (index < 0 || index >= Host->Info.BlockTableCountMax) { Debug::Error("Invalid index while reading from block table"); return NULL_BLKENTRY; }
        uint32_t sector = GetSectorFromIndex(index);
        uint32_t offset = GetOffsetFromIndex(sector, index);
        uint8_t* data = (uint8_t*)Core::Heap.Allocate(Core::ATA.SectorSize, true, Memory::HeapType::Array);
        Core::ATA.Read(sector, 1, data);
        FSBlockEntry* entry = (FSBlockEntry*)(data + offset);
        FSBlockEntry output = { entry->Start, entry->Count, entry->State, { 0 } };
        Core::Heap.Free(data);
        return output;
    }

    bool FSBlockTable::Write(int index, FSBlockEntry entry)
    {
        Host->ReadInfo();
        if (index < 0 || index >= Host->Info.BlockTableCountMax) { Debug::Error("Invalid index while reading from block table"); return false; }
        uint32_t sector = GetSectorFromIndex(index);
        uint32_t offset = GetOffsetFromIndex(sector, index);
        uint8_t* data = (uint8_t*)Core::Heap.Allocate(Core::ATA.SectorSize, true, Memory::HeapType::Array);
        Core::ATA.Read(sector, 1, data);
        FSBlockEntry* temp = (FSBlockEntry*)(data + offset);
        temp->Start         = entry.Start;
        temp->Count         = entry.Count;
        temp->State         = entry.State;
        memcpy(temp->Padding, entry.Padding, sizeof(entry.Padding));
        Core::ATA.Write(sector, 1, data);
        Core::Heap.Free(data);
        return true;
    }

    bool FSBlockTable::Copy(FSBlockEntry dest, FSBlockEntry src)
    {
        uint8_t* data = (uint8_t*)Core::Heap.Allocate(Core::ATA.SectorSize, true, Memory::HeapType::Array);
        for (uint32_t sec = 0; sec < src.Count; sec++)
        {
            Core::ATA.Read(src.Start + sec, 1, data);
            Core::ATA.Write(dest.Start + sec, 1, data);
        }
        Core::Heap.Free(data);
        return true;
    }

    bool FSBlockTable::Fill(FSBlockEntry entry, uint8_t value)
    {
        uint8_t* data = (uint8_t*)Core::Heap.Allocate(Core::ATA.SectorSize, true, Memory::HeapType::Array);
        for (uint32_t sec = 0; sec < Host->Info.BlockTableSectorCount; sec++)
        {
            Core::ATA.Read(Host->Info.BlockTableStart + sec, 1, data);

            for (uint32_t i = 0; i < Core::ATA.SectorSize; i += sizeof(FSBlockEntry))
            {
                FSBlockEntry* temp = (FSBlockEntry*)(data + i);
                if (temp->Start == 0 || temp->Count == 0) { continue; }

                if (temp->Start == entry.Start && temp->Count == entry.Count && temp->State == entry.State)
                {
                    Core::ATA.Fill(temp->Start, temp->Count, data);
                    Core::Heap.Free(data);
                    return true;
                }
            }
        }

        Debug::Error("Unable to fill block entry");
        Core::Heap.Free(data);
        return false;
    }

    FSBlockEntry FSBlockTable::Allocate(uint32_t sectors)
    {
        if (sectors == 0) { return NULL_BLKENTRY; }

        Host->ReadInfo();
        uint8_t* data = (uint8_t*)Core::Heap.Allocate(Core::ATA.SectorSize, true, Memory::HeapType::Array);
        int index = 0;
        for (uint32_t sec = 0; sec < Host->Info.BlockTableSectorCount; sec++)
        {
            Core::ATA.Read(Host->Info.BlockTableStart + sec, 1, data);

            for (uint32_t i = 0; i < Core::ATA.SectorSize; i += sizeof(FSBlockEntry))
            {
                FSBlockEntry* entry = (FSBlockEntry*)(data + i);
                if (!ValidateSector(entry->Start)) { index++; continue; }
                if (entry->Count == sectors && entry->State == FSSTATE_FREE)
                {
                    FSBlockEntry output;
                    entry->State = FSSTATE_USED;
                    memcpy(&output, entry, sizeof(FSBlockEntry));
                    Write(index, output);
                    Debug::Info("Allocated block: START: 0x%8x, STATE = 0x%2x, COUNT = 0x%8x", output.Start, output.State, output.Count);
                    Core::Heap.Free(data);
                    return output;
                }

                index++;
            }
        }

        Core::ATA.Read(Host->Info.BlockTableStart, 1, data);
        FSBlockEntry* mass = (FSBlockEntry*)data;

        mass->Start += sectors;
        mass->Count -= sectors;
        mass->State  = FSSTATE_FREE;
        Core::ATA.Write(Host->Info.BlockTableStart, 1, data);
        FSBlockEntry output = CreateEntry(mass->Start - sectors, sectors, FSSTATE_USED);
        Debug::Info("Allocated block: START: 0x%8x, STATE = 0x%2x, COUNT = 0x%8x", output.Start, output.State, output.Count);
        Core::Heap.Free(data);
        return output;
    }

    bool FSBlockTable::Free(FSBlockEntry entry)
    {
        Host->ReadInfo();
        uint8_t* data = (uint8_t*)Core::Heap.Allocate(Core::ATA.SectorSize, true, Memory::HeapType::Array);
        int index = 0;
        for (uint32_t sec = 0; sec < Host->Info.BlockTableSectorCount; sec++)
        {
            Core::ATA.Read(Host->Info.BlockTableStart + sec, 1, data);

            for (uint32_t i = 0; i < Core::ATA.SectorSize; i += sizeof(FSBlockEntry))
            {
                FSBlockEntry* temp = (FSBlockEntry*)(data + i);
                if (temp->Start == 0) { index++; continue; }
                if (temp->Start == entry.Start && temp->Count == entry.Count && temp->State == entry.State)
                {
                    temp->State = FSSTATE_FREE;
                    Core::ATA.Write(Host->Info.BlockTableStart + sec, 1, data);
                    Debug::Info("Freed block: START: 0x%8x, STATE = 0x%2x, COUNT = 0x%8x", temp->Start, temp->State, temp->Count);
                    MergeFreeBlocks();
                    return true;
                }
            }
            index++;
        }

        Debug::Error("Unable to free block START: %d, STATE = 0x%2x, COUNT = %d", entry.Start, entry.State, entry.Count);
        return false;
    }

    void FSBlockTable::MergeFreeBlocks()
    {
        Host->ReadInfo();
        FSBlockEntry mass = Get(0);
        uint8_t* data =(uint8_t*)Core::Heap.Allocate(Core::ATA.SectorSize, true, Memory::HeapType::Array);
        int index = 0;
        for (uint32_t sec = 0; sec < Host->Info.BlockTableSectorCount; sec++)
        {
            Core::ATA.Read(Host->Info.BlockTableStart + sec, 1, data);

            for (uint32_t i = 0; i < Core::ATA.SectorSize; i += sizeof(FSBlockEntry))
            {
                FSBlockEntry* temp = (FSBlockEntry*)(data + i);
                if (index == 0) { index++; continue; }
                if (temp->Start == 0 || temp->Count == 0) { index++; continue; }
                
                if (temp->State == FSSTATE_FREE)
                {
                    FSBlockEntry nearest = GetNearest(*temp);
                    if (nearest.Start > 0 && nearest.Count > 0 && nearest.Start != temp->Start && nearest.Start != mass.Start && nearest.State == FSSTATE_FREE)
                    {
                        if (temp->Start > nearest.Start) { temp->Start = nearest.Start; }
                        temp->Count += nearest.Count;
                        Core::ATA.Write(Host->Info.BlockTableStart + sec, 1, data);
                        if (!DeleteEntry(nearest)) { Debug::Error("Error deleting entry while merging"); return; }
                        Core::ATA.Read(Host->Info.BlockTableStart + sec, 1, data);
                    }
                }
                index++;
            }
        }
        Host->WriteInfo();

        mass = Get(0);

        Host->ReadInfo();
        index = 0;
        for (uint32_t sec = 0; sec < Host->Info.BlockTableSectorCount; sec++)
        {
            Core::ATA.Read(Host->Info.BlockTableStart + sec, 1, data);

            for (uint32_t i = 0; i < Core::ATA.SectorSize; i += sizeof(FSBlockEntry))
            {
                FSBlockEntry* temp = (FSBlockEntry*)(data + i);
                if (index == 0) { index++; continue; }
                if (temp->Start == 0 || temp->Count == 0) { index++; continue; }

                if (temp->Start + temp->Count == mass.Start && temp->State == FSSTATE_FREE)
                {
                    mass.Start = temp->Start;
                    mass.Count += temp->Count;
                    mass.State = FSSTATE_FREE;
                    Write(0, mass);
                    Core::ATA.Read(Host->Info.BlockTableStart + sec, 1, data);
                    memset(temp, 0, sizeof(FSBlockEntry));
                    Core::ATA.Write(Host->Info.BlockTableStart + sec, 1, data);
                    Host->WriteInfo();
                    break;
                }
                index++;
            }
        }

        Core::Heap.Free(data);
        Host->WriteInfo();
    }

    FSBlockEntry FSBlockTable::GetNearest(FSBlockEntry entry)
    {
        if (entry.Start == 0) { return NULL_BLKENTRY; }
        if (entry.Count == 0) { return NULL_BLKENTRY; }

        Host->ReadInfo();
        uint8_t* data = (uint8_t*)Core::Heap.Allocate(Core::ATA.SectorSize, true, Memory::HeapType::Array);
        int index = 0;
        for (uint32_t sec = 0; sec < Host->Info.BlockTableSectorCount; sec++)
        {
            Core::ATA.Read(Host->Info.BlockTableStart + sec, 1, data);

            for (uint32_t i = 0; i < Core::ATA.SectorSize; i += sizeof(FSBlockEntry))
            {
                if (index == 0) { index++; continue; }
                FSBlockEntry* temp = (FSBlockEntry*)(data + i);
                if (temp->Start == 0 || temp->Count == 0) { index++; continue; }

                if ((temp->Start + temp->Count == entry.Start && entry.State == FSSTATE_FREE) ||
                    (entry.Start - entry.Count == temp->Start && entry.State == FSSTATE_FREE))
                    {
                        FSBlockEntry output;
                        memcpy(&output, temp, sizeof(FSBlockEntry));
                        Core::Heap.Free(data);
                        return output;
                    }

                index++;
            }
        }
        return NULL_BLKENTRY;
    }

    FSBlockEntry FSBlockTable::CreateEntry(uint32_t start, uint32_t count, uint8_t state)
    {
        int i = GetFreeIndex();
        if (i < 0 || i >= Host->Info.BlockTableCountMax) { Debug::Error("Maximum amount of block entries reached"); return NULL_BLKENTRY; }
        FSBlockEntry entry;
        entry.Start = start;
        entry.Count = count;
        entry.State = state;
        memset(entry.Padding, 0, sizeof(entry.Padding));
        Write(i, entry);
        Host->Info.BlockTableCount++;
        Host->WriteInfo();
        Debug::Info("Created fs block: START: %d, STATE = 0x%2x, COUNT = %d", entry.Start, entry.State, entry.Count);
        return entry;
    }

    bool FSBlockTable::DeleteEntry(FSBlockEntry entry)
    {
        Host->ReadInfo();
        uint8_t* data = (uint8_t*)Core::Heap.Allocate(Core::ATA.SectorSize, true, Memory::HeapType::Array);
        for (uint32_t sec = 0; sec < Host->Info.BlockTableSectorCount; sec++)
        {
            Core::ATA.Read(Host->Info.BlockTableStart + sec, 1, data);

            for (uint32_t i = 0; i < Core::ATA.SectorSize; i += sizeof(FSBlockEntry))
            {
                FSBlockEntry* temp = (FSBlockEntry*)(data + i);
                if (temp->Start == 0 || temp->Count == 0) { continue; }

                if (temp->Start == entry.Start && temp->Count == entry.Count && temp->State == entry.State)
                {
                    Debug::Info("Delete block: START: 0x%8x, STATE = 0x%2x, COUNT = 0x%8x", entry.Start, entry.State, entry.Count);
                    memset(temp, 0, sizeof(FSBlockEntry));
                    Core::ATA.Write(Host->Info.BlockTableStart + sec, 1, data);
                    Host->Info.BlockTableCount--;
                    Host->WriteInfo();
                    return true;
                }
            }
        }

        Debug::Error("Unable to delete block");
        return false;
    }
    
    FSBlockEntry FSBlockTable::Get(int index)
    {
        if (index < 0 || index >+ Host->Info.BlockTableCountMax) { return NULL_BLKENTRY; }

        Host->ReadInfo();
        uint8_t* data = (uint8_t*)Core::Heap.Allocate(Core::ATA.SectorSize, true, Memory::HeapType::Array);
        int temp_index = 0;
        for (uint32_t sec = 0; sec < Host->Info.BlockTableSectorCount; sec++)
        {
            Core::ATA.Read(Host->Info.BlockTableStart + sec, 1, data);

            for (uint32_t i = 0; i < Core::ATA.SectorSize; i += sizeof(FSBlockEntry))
            {
                FSBlockEntry* temp = (FSBlockEntry*)(data + i);
                if (temp_index == index) 
                { 
                    FSBlockEntry output;
                    memcpy(&output, temp, sizeof(FSBlockEntry));
                    Core::Heap.Free(data);
                    return output; 
                }
                temp_index++;
            }
        }

        Debug::Error("Unable to get block entry at index %d", index);
        Core::Heap.Free(data);
        return NULL_BLKENTRY;
    }

    bool FSBlockTable::ValidateSector(uint32_t sector)
    {
        if (sector < Host->Info.BlockTableStart || sector >= Host->Info.BlockTableSectorCount) { return false; }
        return true;
    }

    int FSBlockTable::GetIndex(FSBlockEntry entry)
    {
        Host->ReadInfo();
        uint8_t* data = (uint8_t*)Core::Heap.Allocate(Core::ATA.SectorSize, true, Memory::HeapType::Array);
        int index = 0;
        for (uint32_t sec = 0; sec < Host->Info.BlockTableSectorCount; sec++)
        {
            Core::ATA.Read(Host->Info.BlockTableStart + sec, 1, data);

            for (uint32_t i = 0; i < Core::ATA.SectorSize; i += sizeof(FSBlockEntry))
            {
                FSBlockEntry* temp = (FSBlockEntry*)(data + i);
                if (temp->Start == 0 || temp->Count == 0) { continue; }
                if (temp->Start == entry.Start && temp->Count == entry.Count && temp->State == entry.State) { Core::Heap.Free(data); return index; }
                index++;
            }
        }
        Core::Heap.Free(data);
        return -1;
    }

    int FSBlockTable::GetFreeIndex()
    {
        Host->ReadInfo();
        uint8_t* data = (uint8_t*)Core::Heap.Allocate(Core::ATA.SectorSize, true, Memory::HeapType::Array);
        int index = 0;
        for (uint32_t sec = 0; sec < Host->Info.BlockTableSectorCount; sec++)
        {
            Core::ATA.Read(Host->Info.BlockTableStart + sec, 1, data);

            for (uint32_t i = 0; i < Core::ATA.SectorSize; i += sizeof(FSBlockEntry))
            {
                FSBlockEntry* entry = (FSBlockEntry*)(data + i);
                if (entry->Start == 0 && entry->Count == 0 && entry->State == 0) { Core::Heap.Free(data); return index; }
                index++;
            }
        }
        Core::Heap.Free(data); 
        return -1;
    }

    uint32_t FSBlockTable::GetSectorFromIndex(int index)
    {
        if (index == 0) { index = 1; }
        uint32_t sec = FS_SECTOR_BLKS;
        uint32_t offset_bytes = (index * sizeof(FSBlockEntry));
        sec += (offset_bytes / Core::ATA.SectorSize);
        return sec;
    }

    uint32_t FSBlockTable::GetOffsetFromIndex(uint32_t sector, int index)
    {
        uint32_t offset_bytes = (index * sizeof(FSBlockEntry));
        uint32_t val = offset_bytes % 512;
        return val;
    }

    // ----------------------------------------------------------------------------

    void FSFileTable::Init(FSHost* host)
    {
        Host = host;
    }

    void FSFileTable::Print(DebugMode mode)
    {
        Host->ReadInfo();
        uint8_t* data = (uint8_t*)Core::Heap.Allocate(Core::ATA.SectorSize, true, Memory::HeapType::Array);

        Debug::PrintFormatted("------ FILE TABLE ----------------------------\n", mode);

        int index = 0;
        for (uint32_t sec = 0; sec < Host->Info.FileTableSectorCount; sec++)
        {
            Core::ATA.Read(Host->Info.FileTableStart + sec, 1, data);

            for (uint32_t i = 0; i < Core::ATA.SectorSize; i += sizeof(FSDirectory))
            {
                FSDirectory* entry = (FSDirectory*)(data + i);

                if (entry->Type != FSTYPE_NULL)
                {
                    Debug::PrintFormatted("INDEX: 0x%8x PARENT: 0x%8x TYPE: 0x%2x STATUS: 0x%2x NAME: %s\n", mode, index, entry->ParentIndex, entry->Type, entry->Status, entry->Name);
                }
                index++;
            }
        }

        Debug::WriteChar('\n');
        Core::Heap.Free(data); 
    }

    FSDirectory FSFileTable::ReadDirectory(int index)
    {
        Host->ReadInfo();
        if (index < 0 || index >= Host->Info.FileTableCountMax) { Debug::Error("Invalid index while reading directory entry"); return NULL_DIR; }
        uint32_t sector = GetSectorFromIndex(index);
        uint32_t offset = GetOffsetFromIndex(sector, index);
        uint8_t* data = (uint8_t*)Core::Heap.Allocate(Core::ATA.SectorSize, true, Memory::HeapType::Array);
        Core::ATA.Read(sector, 1, data);
        FSDirectory* entry = (FSDirectory*)(data + offset);
        FSDirectory output;
        memcpy(&output, entry, sizeof(FSDirectory));
        Core::Heap.Free(data);
        return output;
    }

    FSFile FSFileTable::ReadFile(int index)
    {
        Host->ReadInfo();
        if (index < 0 || index >= Host->Info.FileTableCountMax) { Debug::Error("Invalid index while reading directory entry"); return NULL_FILE; }
        uint32_t sector = GetSectorFromIndex(index);
        uint32_t offset = GetOffsetFromIndex(sector, index);
        uint8_t* data = (uint8_t*)Core::Heap.Allocate(Core::ATA.SectorSize, true, Memory::HeapType::Array);
        Core::ATA.Read(sector, 1, data);
        FSFile* entry = (FSFile*)(data + offset);
        FSFile output;
        memcpy(&output, entry, sizeof(FSFile));
        Core::Heap.Free(data); 
        return output;
    }

    bool FSFileTable::WriteDirectory(int index, FSDirectory dir)
    {
        Host->ReadInfo();
        if (index < 0 || index >= Host->Info.FileTableCountMax) { Debug::Error("Invalid index while writing directory entry"); return false; }
        uint32_t sector = GetSectorFromIndex(index);
        uint32_t offset = GetOffsetFromIndex(sector, index);
        uint8_t* data = (uint8_t*)Core::Heap.Allocate(Core::ATA.SectorSize, true, Memory::HeapType::Array);
        Core::ATA.Read(sector, 1, data);
        FSDirectory* temp = (FSDirectory*)(data + offset);
        memcpy(temp, &dir, sizeof(FSDirectory));
        Core::ATA.Write(sector, 1, data);
        Core::Heap.Free(data);
    }

    bool FSFileTable::WriteFile(int index, FSFile file)
    {
        Host->ReadInfo();
        if (index < 0 || index >= Host->Info.FileTableCountMax) { Debug::Error("Invalid index while writing file entry"); return false; }
        uint32_t sector = GetSectorFromIndex(index);
        uint32_t offset = GetOffsetFromIndex(sector, index);
        uint8_t* data = (uint8_t*)Core::Heap.Allocate(Core::ATA.SectorSize, true, Memory::HeapType::Array);
        Core::ATA.Read(sector, 1, data);
        FSFile* temp = (FSFile*)(data + offset);
        memcpy(temp, &file, sizeof(FSFile));
        Core::ATA.Write(sector, 1, data);
        Core::Heap.Free(data); 
    }

    FSDirectory FSFileTable::CreateDirectory(FSDirectory dir)
    {
        int i = GetFreeIndex();
        if (i < 0 || i >= Host->Info.FileTableCountMax) { Debug::Error("Invalid index while creating directory entry"); return NULL_DIR; }
        WriteDirectory(i, dir);
        Host->Info.FileTableCount++;
        Host->WriteInfo();
        Debug::Info("Created directory: INDEX = 0x%8x, NAME = %s, PARENT = 0x%8x, TYPE = 0x%2x, STATUS = 0x%2x", i, dir.Name, dir.ParentIndex, dir.Type, dir.Status);
        return dir;
    }

    FSFile FSFileTable::CreateFile(FSFile file)
    {
        int i = GetFreeIndex();
        if (i < 0 || i >= Host->Info.FileTableCountMax) { Debug::Error("Invalid index while creating file entry"); return NULL_FILE; }
        WriteFile(i, file);
        Host->Info.FileTableCount++;
        Host->WriteInfo();
        Debug::Info("Created file: NAME = %s, PARENT = 0x%8x, TYPE = 0x%2x, STATUS = 0x%2x, BLK = 0x%8x, SIZE = %d", file.Name, file.ParentIndex, file.Type, file.Status, file.BlockIndex, file.Size);
        return file;
    }

    bool FSFileTable::DeleteDirectory(FSDirectory dir)
    {
        Host->ReadInfo();
        uint8_t* data = (uint8_t*)Core::Heap.Allocate(Core::ATA.SectorSize, true, Memory::HeapType::Array);
        int index = 0;
        for (uint32_t sec = 0; sec < Host->Info.FileTableMaxUsed; sec++)
        {
            Core::ATA.Read(Host->Info.FileTableStart + sec, 1, data);

            for (uint32_t i = 0; i < Core::ATA.SectorSize; i += sizeof(FSDirectory))
            {
                FSDirectory* entry = (FSDirectory*)(data + i);

                if (Host->DirectoryEquals(dir, *entry))
                {
                    Debug::Info("Deleted directory: NAME = %s, PARENT = 0x%8x, TYPE = 0x%2x, STATUS = 0x%2x", dir.Name, dir.ParentIndex, dir.Type, dir.Status);
                    memset(entry, 0, sizeof(FSDirectory));
                    Core::ATA.Write(Host->Info.FileTableStart + sec, 1, data);
                    Core::Heap.Free(data); 
                    return true;
                }
                index++;
            }
        }

        Debug::Error("Unable to delete directory");
        Core::Heap.Free(data); 
        return false;
    }

    bool FSFileTable::DeleteFile(FSFile file)
    {
        Host->ReadInfo();
        uint8_t* data = (uint8_t*)Core::Heap.Allocate(Core::ATA.SectorSize, true, Memory::HeapType::Array);
        int index = 0;
        for (uint32_t sec = 0; sec < Host->Info.FileTableMaxUsed; sec++)
        {
            Core::ATA.Read(Host->Info.FileTableStart + sec, 1, data);

            for (uint32_t i = 0; i < Core::ATA.SectorSize; i += sizeof(FSFile))
            {
                FSFile* entry = (FSFile*)(data + i);

                if (Host->FileEquals(file, *entry))
                {
                    Debug::Info("Deleted file: NAME = %s, PARENT = 0x%8x, TYPE = 0x%2x, STATUS = 0x%2x, SIZE = %d", file.Name, file.ParentIndex, file.Type, file.Status, file.Size);
                    memset(entry, 0, sizeof(FSFile));
                    Core::ATA.Write(Host->Info.FileTableStart + sec, 1, data);
                    Core::Heap.Free(data); 
                    return true;
                }

                index++;
            }
        }

        Debug::Error("Unable to delete file");
        Core::Heap.Free(data); 
        return false;
    }

    bool FSFileTable::ValidateSector(uint32_t sector)
    {
        if (sector < Host->Info.FileTableStart || sector >= Host->Info.FileTableStart + Host->Info.FileTableSectorCount) { return false; }
        return true;
    }

    int FSFileTable::GetDirectoryIndex(FSDirectory dir)
    {
        Host->ReadInfo();
        uint8_t* data = (uint8_t*)Core::Heap.Allocate(Core::ATA.SectorSize, true, Memory::HeapType::Array);
        int index = 0;
        for (uint32_t sec = 0; sec < Host->Info.FileTableMaxUsed; sec++)
        {
            Core::ATA.Read(Host->Info.FileTableStart + sec, 1, data);

            for (uint32_t i = 0; i < Core::ATA.SectorSize; i += sizeof(FSDirectory))
            {
                FSDirectory* entry = (FSDirectory*)(data + i);
                if (Host->DirectoryEquals(*entry, dir)) { Core::Heap.Free(data); return index; }
                index++;
            }
        }
        Core::Heap.Free(data);
        return -1;
    }

    int FSFileTable::GetFileIndex(FSFile file)
    {
        Host->ReadInfo();
        uint8_t* data = (uint8_t*)Core::Heap.Allocate(Core::ATA.SectorSize, true, Memory::HeapType::Array);
        int index = 0;
        for (uint32_t sec = 0; sec < Host->Info.FileTableMaxUsed; sec++)
        {
            Core::ATA.Read(Host->Info.FileTableStart + sec, 1, data);

            for (uint32_t i = 0; i < Core::ATA.SectorSize; i += sizeof(FSFile))
            {
                FSFile* entry = (FSFile*)(data + i);
                if (Host->FileEquals(*entry, file)) { Core::Heap.Free(data); return index; }
                index++;
            }
        }
        Core::Heap.Free(data);
        return -1;
    }

    int FSFileTable::GetFreeIndex()
    {
        Host->ReadInfo();
        uint8_t* data = (uint8_t*)Core::Heap.Allocate(Core::ATA.SectorSize, true, Memory::HeapType::Array);
        int index = 0;
        for (uint32_t sec = 0; sec < Host->Info.FileTableSectorCount; sec++)
        {
            Core::ATA.Read(Host->Info.FileTableStart + sec, 1, data);

            for (uint32_t i = 0; i < Core::ATA.SectorSize; i += sizeof(FSFile))
            {
                FSFile* entry = (FSFile*)(data + i);
                if (entry->Type == FSTYPE_NULL) { Core::Heap.Free(data);  return index; }
                index++;
            }
        }

        Core::Heap.Free(data);
        return -1;
    }

    uint32_t FSFileTable::GetSectorFromIndex(int index)
    {
        if (index == 0) { index = 1; }
        Host->ReadInfo();
        uint32_t sec = Host->Info.FileTableStart;
        uint32_t offset_bytes = (index * sizeof(FSFile));
        sec += (offset_bytes / Core::ATA.SectorSize);
        return sec;
    }

    uint32_t FSFileTable::GetOffsetFromIndex(uint32_t sector, int index)
    {
        uint32_t offset_bytes = (index * sizeof(FSFile));
        uint32_t val = offset_bytes % 512;
        return val;
    }

    // -----------------------------------------------------------------------------------------

    void FSHost::Mount()
    {
        ReadInfo();

        BlockEntries.Init(this);
        FileEntries.Init(this);
        RootDir = FileEntries.ReadDirectory(0);

        WriteInfo();
        Debug::OK("Mounted file system");
    }

    void FSHost::Unmount()
    {

    }

    void FSHost::Format(uint32_t size, bool wipe)
    {
        Debug::Info("Fomatting disk...");
        if (wipe) { Wipe(size); }

        // generate info block
        CreateInfo(size);
        ReadInfo();

        // create mass block entry
        BlockEntries.Mass.Start = Info.BlockDataStart;
        BlockEntries.Mass.Count = Info.BlockDataSectorCount;
        BlockEntries.Mass.State = FSSTATE_FREE;
        memset(BlockEntries.Mass.Padding, 0, sizeof(BlockEntries.Mass.Padding));
        BlockEntries.Write(0, BlockEntries.Mass);
        BlockEntries.Mass = BlockEntries.Read(0);
        Debug::Info("Created mass block: START: %d, STATE = 0x%2x, COUNT = %d", BlockEntries.Mass.Start, BlockEntries.Mass.State, BlockEntries.Mass.Count);

        // create files block entry and update info
        ReadInfo();
        Info.FileTableCountMax = 16384;
        Info.FileTableCount    = 0;
        Info.FileTableSectorCount = (Info.FileTableCountMax * sizeof(FSFile)) / Core::ATA.SectorSize;
        WriteInfo();
        BlockEntries.Files = BlockEntries.Allocate(Info.FileTableSectorCount);
        Info.FileTableStart = BlockEntries.Files.Start;
        WriteInfo();

        // create root directory
        CreateRoot("VOS");

        // finished
        Debug::OK("Finished formatting disk");
    }

    void FSHost::Wipe(uint32_t size)
    {
        Debug::Info("Started wiping disk...");
        uint32_t sectors = size / Core::ATA.SectorSize;
        uint8_t* data = (uint8_t*)Core::Heap.Allocate(Core::ATA.SectorSize, true, Memory::HeapType::Array);
        memset(data, 0, Core::ATA.SectorSize);

        for (uint64_t i = 0; i < sectors; i++) { Core::ATA.Write(i, 1, data); }

        Debug::OK("Finished wiping disk");
        Core::Heap.Free(data);
    }

    void FSHost::CreateRoot(char* label)
    {
        strcpy(RootDir.Name, label);
        RootDir.Status = 0xFF;
        RootDir.ParentIndex = 0;
        RootDir.Type = FSTYPE_DIR;
        memset(RootDir.Padding, 0, sizeof(RootDir.Padding));
        FileEntries.WriteDirectory(0, RootDir);

        FSDirectory rfd = FileEntries.ReadDirectory(0);
        Debug::Info("Created root: NAME = %s, PARENT = 0x%8x, TYPE = 0x%2x, STATUS = 0x%2x", rfd.Name, rfd.ParentIndex, rfd.Type, rfd.Status);
    }

    void FSHost::CreateInfo(uint32_t size)
    {
        // disk info
        Info.SectorCount        = size / Core::ATA.SectorSize;
        Info.BytesPerSector    = Core::ATA.SectorSize;

        // block table
        Info.BlockTableStart     = FS_SECTOR_BLKS;
        Info.BlockTableCount     = 0;
        Info.BlockTableCountMax = 65536;
        Info.BlockTableSectorCount = (Info.BlockTableCountMax * sizeof(FSBlockEntry)) / Core::ATA.SectorSize;

        // block data
        Info.BlockDataStart = Info.BlockTableStart + Info.BlockTableSectorCount + 4;
        Info.BlockDataSectorCount = Info.SectorCount - (Info.BlockTableSectorCount + 8);
        Info.BlockDataUsed = 0;

        // write to disk
        WriteInfo();

        // finished
        Debug::Info("Created new info block");
    }

    void FSHost::ReadInfo()
    {
        uint8_t* sec = (uint8_t*)Core::Heap.Allocate(Core::ATA.SectorSize, true, Memory::HeapType::Array);
        Core::ATA.Read(FS_SECTOR_INFO, 1, sec);
        memcpy(&Info, sec, sizeof(FSInfo));
        Core::Heap.Free(sec);
    }

    void FSHost::WriteInfo()
    {
        uint8_t* sec = (uint8_t*)Core::Heap.Allocate(Core::ATA.SectorSize, true, Memory::HeapType::Array);
        memset(sec, 0, Core::ATA.SectorSize);

        uint8_t* data = (uint8_t*)Core::Heap.Allocate(Core::ATA.SectorSize, true, Memory::HeapType::Array);
        int index = 0;
        for (uint32_t sec = 0; sec < Info.FileTableSectorCount; sec++)
        {
            Core::ATA.Read(Info.FileTableStart + sec, 1, data);

            bool sec_contains = false;
            for (uint32_t i = 0; i < Core::ATA.SectorSize; i += sizeof(FSDirectory))
            {
                FSDirectory* entry = (FSDirectory*)(data + i);
                if (entry->Type != FSTYPE_NULL) { sec_contains = true; }
            }
            if (sec_contains) { index = sec; }
        }
        Core::Heap.Free(data);
        Info.FileTableMaxUsed = index + 1;
        
        memcpy(sec, &Info, sizeof(FSInfo));

        

        Core::ATA.Write(FS_SECTOR_INFO, 1, sec);
        Core::Heap.Free(sec);
    }

    FSInfo FSHost::GetInfo() { return Info; }

    bool FSHost::DirectoryEquals(FSDirectory a, FSDirectory b)
    {
        if (strcmp(a.Name, b.Name)) { return false; }
        if (a.ParentIndex != b.ParentIndex) { return false; }
        if (a.Status != b.Status) { return false; }
        if (a.Type != b.Type) { return false; }
        return true;
    }

    bool FSHost::FileEquals(FSFile a, FSFile b)
    {
        if (strcmp(a.Name, b.Name)) { return false; }
        if (a.ParentIndex != b.ParentIndex) { return false; }
        if (a.Status != b.Status) { return false; }
        if (a.Type != b.Type) { return false; }
        if (a.BlockIndex != b.BlockIndex) { return false; }
        if (a.Size != b.Size) { return false; }
        return true;
    }

    FSDirectory FSHost::GetParentFromPath(char* path)
    {
        if (path == NULL) { return NULL_DIR; }
        if (strlen(path) == 0) { return NULL_DIR; }

        int    args_count = 0;
        char** args = strsplit((char*)path, '/', &args_count);

        if (args_count <= 2 && path[0] == '/') { Core::Heap.FreeArray((void**)args, args_count); return RootDir; }

        if (args_count > 1)
        {
            ReadInfo();
            int32_t index = 0;
            uint32_t p = 0;
            FSDirectory dir_out = NULL_DIR;

            for (uint32_t arg = 0; arg < args_count - 1; arg++)
            {
                dir_out = NULL_DIR;
                index = 0;

                if (args[arg] != NULL && strlen(args[arg]) > 0)
                {
                    uint8_t* data = (uint8_t*)Core::Heap.Allocate(Core::ATA.SectorSize, true, Memory::HeapType::Array);
                    for (uint32_t sec = 0; sec < Info.FileTableMaxUsed; sec++)
                    {
                        Core::ATA.Read(Info.FileTableStart + sec, 1, data);

                        for (uint32_t i = 0; i < Core::ATA.SectorSize; i += sizeof(FSDirectory))
                        {
                            FSDirectory* dir = (FSDirectory*)(data + i);

                            if (dir->Type == FSTYPE_DIR && dir->ParentIndex == (uint32_t)p && !strcmp(dir->Name, args[arg]))
                            {
                                p = index;
                                dir_out = *dir;
                            }

                            index++;
                        }
                    }
                    Core::Heap.Free(data);
                }
            }

            if (dir_out.Type == FSTYPE_DIR) { Core::Heap.FreeArray((void**)args, args_count); return dir_out; }
        }

        Debug::Error("Unable to locate parent of %s", path);
        Core::Heap.FreeArray((void**)args, args_count);
        return NULL_DIR;
    }

    FSDirectory FSHost::GetDirectoryByName(char* path)
    {
        if (path == NULL) { return NULL_DIR; }
        if (strlen(path) == 0) { return NULL_DIR; }

        ReadInfo();

        if (!strcmp(path, "/"))
        {
            FSDirectory output;
            memcpy(&output, &RootDir, sizeof(FSDirectory));
            return output;
        }

        FSDirectory parent = GetParentFromPath(path);
        if (parent.Type != FSTYPE_DIR) { Debug::Error("Parent was null while getting directory by name"); return NULL_DIR; }

        int args_count = 0;
        char** args = strsplit((char*)path, '/', &args_count);

        if (args_count == 0) { Core::Heap.FreeArray((void**)args, args_count); Debug::Error("Args was null while getting directory by name"); return NULL_DIR; }

        char* dirname;
        int xx = args_count - 1;
        while (args[xx] != NULL)
        {
            if (args[xx] != NULL && strlen(args[xx]) > 0) { dirname = args[xx]; break; }
            if (xx == 0) { break; }
            xx--;
        }
        if (strlen(dirname) == 0 || dirname == NULL) { Debug::Error("Unable to get name while getting directory by name"); return NULL_DIR; }

        uint32_t parent_index = GetDirectoryIndex(parent);

        uint8_t* data = (uint8_t*)Core::Heap.Allocate(Core::ATA.SectorSize, true, Memory::HeapType::Array);
        int index = 0;
        for (uint32_t sec = 0; sec < Info.FileTableMaxUsed; sec++)
        {
            Core::ATA.Read(Info.FileTableStart + sec, 1, data);

            for (uint32_t i = 0; i < Core::ATA.SectorSize; i += sizeof(FSDirectory))
            {
                FSDirectory* entry = (FSDirectory*)(data + i);
                if (entry->Type == FSTYPE_DIR && entry->ParentIndex == parent_index && !strcmp(entry->Name, dirname))
                {
                    FSDirectory output;
                    memcpy(&output, entry, sizeof(FSDirectory));
                    Core::Heap.FreeArray((void**)args, args_count);
                    Core::Heap.Free(data);
                    return output;
                }
                index++;
            }
        }

        Core::Heap.FreeArray((void**)args, args_count);
        Core::Heap.Free(data);
        return NULL_DIR;
    }

    FSFile FSHost::GetFileByName(char* path)
    {
        if (path == NULL) { return NULL_FILE; }
        if (strlen(path) == 0) { return NULL_FILE; }

        ReadInfo();

        FSDirectory parent = GetParentFromPath(path);
        if (parent.Type != FSTYPE_DIR) { Debug::Error("Parent was null while getting file by name"); return NULL_FILE; }

        int args_count = 0;
        char** args = strsplit((char*)path, '/', &args_count);

        if (args_count == 0) { Core::Heap.FreeArray((void**)args, args_count); Debug::Error("Args was null while getting file by name"); return NULL_FILE; }

        char* filename;
        int xx = args_count - 1;
        while (args[xx] != NULL)
        {
            if (args[xx] != NULL && strlen(args[xx]) > 0) { filename = args[xx]; break; }
            if (xx == 0) { break; }
            xx--;
        }
        if (strlen(filename) == 0 || filename == NULL) { Core::Heap.FreeArray((void**)args, args_count); Debug::Error("Unable to get name while getting file by name"); return NULL_FILE; }

        uint32_t parent_index = GetDirectoryIndex(parent);

        uint8_t* data = (uint8_t*)Core::Heap.Allocate(Core::ATA.SectorSize, true, Memory::HeapType::Array);
        int index = 0;
        for (uint32_t sec = 0; sec < Info.FileTableMaxUsed; sec++)
        {
            Core::ATA.Read(Info.FileTableStart + sec, 1, data);

            for (uint32_t i = 0; i < Core::ATA.SectorSize; i += sizeof(FSFile))
            {
                FSFile* entry = (FSFile*)(data + i);
                if (entry->Type == FSTYPE_FILE && entry->ParentIndex == parent_index && !strcmp(entry->Name, filename))
                {
                    FSFile output = *entry;
                    Core::Heap.FreeArray((void**)args, args_count);
                    Core::Heap.Free(data);
                    return output;
                }
                index++;
            }
        }

        Core::Heap.FreeArray((void**)args, args_count);
        Core::Heap.Free(data);
        return NULL_FILE;
    }

    int FSHost::GetDirectoryIndex(FSDirectory dir)
    {
        ReadInfo();
        uint8_t* data = (uint8_t*)Core::Heap.Allocate(Core::ATA.SectorSize, true, Memory::HeapType::Array);
        int index = 0;
        for (uint32_t sec = 0; sec < Info.FileTableMaxUsed; sec++)
        {
            Core::ATA.Read(Info.FileTableStart + sec, 1, data);

            for (uint32_t i = 0; i < Core::ATA.SectorSize; i += sizeof(FSDirectory))
            {
                FSDirectory* entry = (FSDirectory*)(data + i);
                if (DirectoryEquals(*entry, dir)) { Core::Heap.Free(data); return index; }
                index++;
            }
        }
        Core::Heap.Free(data);
        return -1;
    }

    int FSHost::GetFileIndex(FSFile file)
    {
        ReadInfo();
        uint8_t* data = (uint8_t*)Core::Heap.Allocate(Core::ATA.SectorSize, true, Memory::HeapType::Array);
        int index = 0;
         for (uint32_t sec = 0; sec < Info.FileTableMaxUsed; sec++)
        {
            Core::ATA.Read(Info.FileTableStart + sec, 1, data);

            for (uint32_t i = 0; i < Core::ATA.SectorSize; i += sizeof(FSFile))
            {
                FSFile* entry = (FSFile*)(data + i);
                if (FileEquals(*entry, file)) { Core::Heap.Free(data); return index; }
                index++;
            }
        }
        Core::Heap.Free(data);
        return -1;
    }

    char* FSHost::GetNameFromPath(char* path)
    {
        if (path == NULL) { return NULL; }
        if (strlen(path) == 0) { return NULL; }

        if (!strcmp(path, "/"))
        {
            char* rootname = (char*)Core::Heap.Allocate(strlen(RootDir.Name) + 1, true, Memory::HeapType::String);
            strcpy(rootname, RootDir.Name);
            return rootname;
        }

        int args_count = 0;
        char** args = strsplit((char*)path, '/', &args_count);

        if (args_count == 0) { Core::Heap.FreeArray((void**)args, args_count); Debug::Error("Args was null while getting name from path"); return NULL; }

        if (args_count == 2 && path[0] == '/')
        {
            char* rootname = (char*)Core::Heap.Allocate(strlen(args[1]) + 1, true, Memory::HeapType::String);
            strcpy(rootname, args[1]);
            Core::Heap.FreeArray((void**)args, args_count);
            return rootname;
        }

        char* filename;
        int xx = args_count - 1;
        while (args[xx] != NULL)
        {
            if (args[xx] != NULL && strlen(args[xx]) > 0) { filename = args[xx]; break; }
            if (xx == 0) { break; }
            xx--;
        }

        char* output = (char*)Core::Heap.Allocate(strlen(filename) + 1, true, Memory::HeapType::String);
        strcpy(output, filename);
        Core::Heap.FreeArray((void**)args, args_count);
        return output;
    }

    char* FSHost::GetParentPathFromPath(char* path)
    {
        if (path == NULL) { return NULL; }
        if (strlen(path) == 0) { return NULL; }

        char* output = (char*)Core::Heap.Allocate(strlen(path) + 16, true, Memory::HeapType::String);

        if (!strcmp(path, "/")) { Core::Heap.Free(output); return NULL; }

        int args_count = 0;
        char** args = strsplit((char*)path, '/', &args_count);   

        if (args_count <= 2 && path[0] == '/') { Core::Heap.FreeArray((void**)args, args_count); strcpy(output, "/"); return output; }

        strcat(output, "/");
        for (int i = 0; i < args_count - 1; i++)
        {
            if (strlen(args[i]) == 0) { continue; }
            strcat(output, args[i]);
            strcat(output, "/");
        }
        
        Core::Heap.FreeArray((void**)args, args_count);
        return output;
    }

    FSFile FSHost::CreateFile(char* path, uint32_t size)
    {
        if (size == 0) { Debug::Error("Cannot create blank file"); return NULL_FILE; }

        FSDirectory parent = GetParentFromPath(path);
        if (parent.Type != FSTYPE_DIR) { Debug::Error("Unable to locate parent while creating file"); return NULL_FILE; }

        uint32_t sectors = GetSectorsFromBytes(size);
        FSBlockEntry blk = BlockEntries.Allocate(sectors);

        // set properties and create file
        FSFile file;
        file.ParentIndex = GetDirectoryIndex(parent);
        file.Type         = FSTYPE_FILE;
        file.Status       = 0x00;
        file.Size         = size;
        file.BlockIndex    = BlockEntries.GetIndex(blk);
        char* name = GetNameFromPath(path);
        strcpy(file.Name, name);
        Core::Heap.Free(name);

        BlockEntries.Fill(blk, 0x00);

        FSFile new_file = FileEntries.CreateFile(file);
        return new_file;
    }

    FSFile FSHost::ReadFile(char* path)
    {
        if (path == NULL) { Debug::Error("Path was null while trying to read file %s", path); return NULL_FILE; }
        if (strlen(path) == 0) { Debug::Error("Path was empty while trying to read file %s", path); return NULL_FILE; }

        FSFile file = GetFileByName(path);
        if (file.Type != FSTYPE_FILE) { Debug::Error("Unable to locate file %s", path); return NULL_FILE; }

        FSBlockEntry blk = BlockEntries.Read(file.BlockIndex);

        uint8_t* data = (uint8_t*)Core::Heap.Allocate(blk.Count * Core::ATA.SectorSize, true, Memory::HeapType::Array);
        for (uint32_t i = 0; i < blk.Count; i++) { Core::ATA.Read(blk.Start + i, 1, data + (i * Core::ATA.SectorSize)); }
        file.Data = data;
        return file;
    }

    bool FSHost::WriteFile(char* path, uint8_t* data, uint32_t len)
    {
        if (path == NULL) { Debug::Error("Path was null while trying to write file %s", path); return false; }
        if (strlen(path) == 0) { Debug::Error("Path was empty while trying to write file %s", path); return false; }

        FSFile tryload = GetFileByName(path);
        if (tryload.Type != FSTYPE_FILE) 
        { 
            Debug::Info("File %s does not exist and will be created", path);
            FSFile new_file = CreateFile(path, len);
            if (new_file.Type != FSTYPE_FILE) { Debug::Error("Unable to create new file %s", path); return false; }
            FSBlockEntry blk = BlockEntries.Read(new_file.BlockIndex);

            uint8_t* secdata = (uint8_t*)Core::Heap.Allocate(Core::ATA.SectorSize, true, Memory::HeapType::Array);
            for (uint32_t i = 0; i < blk.Count; i++)
            {
                memset(secdata, 0, Core::ATA.SectorSize);
                for (uint32_t j = 0; j < Core::ATA.SectorSize; j++)
                {
                    uint32_t offset = (i * Core::ATA.SectorSize) + j;
                    if (offset < len) { secdata[j] = data[offset]; } else { break; }
                }       
                Core::ATA.Write(blk.Start + i, 1, secdata);
            }

            Debug::Info("Written file %s to disk, size = %d", path, new_file.Size);
            Core::Heap.Free(secdata);
        }
        else 
        { 
            int findex = GetFileIndex(tryload);
            FSBlockEntry blk = BlockEntries.Read(tryload.BlockIndex);
            BlockEntries.Free(blk);
            blk = BlockEntries.Allocate(GetSectorsFromBytes(len));
            tryload.BlockIndex = BlockEntries.GetIndex(blk);
            tryload.Size = len;
            FileEntries.WriteFile(findex, tryload);

            uint8_t* secdata = (uint8_t*)Core::Heap.Allocate(Core::ATA.SectorSize, true, Memory::HeapType::Array);
            for (uint32_t i = 0; i < blk.Count; i++)
            {
                memset(secdata, 0, Core::ATA.SectorSize);
                for (uint32_t j = 0; j < Core::ATA.SectorSize; j++)
                {
                    uint32_t offset = (i * Core::ATA.SectorSize) + j;
                    if (offset < len) { secdata[j] = data[offset]; } else { break; }
                }       
                Core::ATA.Write(blk.Start + i, 1, secdata);
            }  

            Debug::Info("Written file %s to disk, size = %d", path, tryload.Size);
            return true;      
        }
        return true;
    }
    
    int ceilnum(float num) 
    {
        int inum = (int)num;
        if (num == (float)inum) { return inum; }
        return inum + 1;
    }

    uint32_t FSHost::GetSectorsFromBytes(uint32_t bytes)
    {
        return (uint32_t)ceilnum((float)bytes / (float)Core::ATA.SectorSize);
    }
}