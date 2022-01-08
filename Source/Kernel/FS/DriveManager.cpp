#include <Kernel/FS/DriveManager.h>
#include <Kernel/Core/Kernel.h>
using namespace HAL;
using namespace System;
using namespace Graphics;

#define DRIVE_INVALID (char)0xFF
#define DRIVE_MAX     128

namespace FileSystem
{
    namespace DriveManager
    {
        // properties
        VirtualDrive* Drives[DRIVE_MAX];
        uint32_t      Count;

        // private methods
        int GetFreeIndex();

        /// @brief Initialize drive manager
        void Init()
        {
            // clear drives
            memset(Drives, 0, sizeof(VirtualDrive*) * DRIVE_MAX);

            // finished
            Debug::Info("Initialized drive manager");
        }

        /// @brief Register new drive @param drive Pointer to drive @return Success of registration
        bool Register(VirtualDrive* drive)
        {
            if (drive == nullptr) { Debug::Error("Tried to register null drive"); return false; }
            int i = GetFreeIndex();
            if (i < 0 || i >= DRIVE_MAX) { Debug::Error("Maximum amount of registered drives reached"); return false; }
            Drives[i] = drive;
            Count++;
            Debug::Info("Registered drive - LETTER: %c, TYPE: 0x%2x, FSTYPE: 0x%2x, LABEL: '%s'", Drives[i]->Letter, Drives[i]->Type, Drives[i]->FSType, Drives[i]->Label);
            return true;
        }

        /// @brief Register new drive @param type Drive type @param letter Character value of drive letter @param label String value of drive label @return Success of registration
        VirtualDrive* Register(DriveType type, FileSystemType fs_type, char letter, char* label)
        {
            int i = GetFreeIndex();
            if (i < 0 || i >= DRIVE_MAX) { Debug::Error("Maximum amount of registered drives reached"); return nullptr; }
            Drives[i] = (VirtualDrive*)Core::Heap.Allocate(sizeof(VirtualDrive), true, Memory::HeapType::Object);
            Drives[i]->Type = type;
            Drives[i]->FSType = fs_type;
            Drives[i]->Letter = letter;
            strcpy(Drives[i]->Label, label);
            Count++;
            Debug::Info("Registered drive - LETTER: %c, TYPE: 0x%2x, FSTYPE: 0x%2x, LABEL: '%s'", Drives[i]->Letter, Drives[i]->Type, Drives[i]->FSType, Drives[i]->Label);
            return Drives[i];
        }

        /// @brief Unregister specified drive @param drive Pointer to drive @return Success of un-registration
        bool Unregister(VirtualDrive* drive, bool free)
        {
            if (drive == nullptr) { return false; }
            for (uint32_t i = 0; i < DRIVE_MAX; i++)
            {
                if (Drives[i] == drive)
                {
                    Debug::Info("Un-registered drive - LETTER: %c, TYPE: 0x%2x, FSTYPE: 0x%2x, LABEL: '%s'", Drives[i]->Letter, Drives[i]->Type, Drives[i]->FSType, Drives[i]->Label);
                    if (free) { Core::Heap.Free(Drives[i]); }
                    Drives[i] = nullptr;
                    return true;
                }
            }
            
            Debug::Error("Unable to un-register drive 0x%8x", (uint32_t)drive);
            return false;
        }

        /// @brief Unregister specified drive @param drive Character value of drive letter @return Success of un-registration
        bool Unregister(char letter, bool free)
        {
            for (uint32_t i = 0; i < DRIVE_MAX; i++)
            {
                if (Drives[i] == nullptr) { continue; }
                if (Drives[i]->Letter == letter)
                {
                    Debug::Info("Un-registered drive - LETTER: %c, TYPE: 0x%2x, FSTYPE: 0x%2x, LABEL: '%s'", Drives[i]->Letter, Drives[i]->Type, Drives[i]->FSType, Drives[i]->Label);
                    if (free) { Core::Heap.Free(Drives[i]); }
                    Drives[i] = nullptr;
                    return true;
                }
            }
            
            Debug::Error("Unable to un-register drive %c", letter);
            return false;
        }

        /// @brief Unregister specified drive @param drive String value of drive label @return Success of un-registration
        bool Unregister(char* label, bool free)
        {
            if (label == nullptr) { return false; }
            for (uint32_t i = 0; i < DRIVE_MAX; i++)
            {
                if (Drives[i] == nullptr) { continue; }
                if (streql(Drives[i]->Label, label))
                {
                    Debug::Info("Un-registered drive - LETTER: %c, TYPE: 0x%2x, FSTYPE: 0x%2x, LABEL: '%s'", Drives[i]->Letter, Drives[i]->Type, Drives[i]->FSType, Drives[i]->Label);
                    if (free) { Core::Heap.Free(Drives[i]); }
                    Drives[i] = nullptr;
                    return true;
                }
            }
            
            Debug::Error("Unable to un-register drive '%s'", label);
            return false;
        }

        int GetFreeIndex()
        {
            for (int i = 0; i < DRIVE_MAX; i++)
            {
                if (Drives[i] == nullptr) { return i; }
            }
            return -1;
        }
    }
}