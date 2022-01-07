#include <Kernel/HAL/Drivers/ATA.h>
#include <Kernel/Core/Kernel.h>
using namespace HAL;
using namespace System;
using namespace Graphics;

// ports
#define ATA_PRIMARY_DATA         0x1F0
#define ATA_PRIMARY_ERR          0x1F1
#define ATA_PRIMARY_SECCOUNT     0x1F2
#define ATA_PRIMARY_LBA_LO       0x1F3
#define ATA_PRIMARY_LBA_MID      0x1F4
#define ATA_PRIMARY_LBA_HI       0x1F5
#define ATA_PRIMARY_DRIVE_HEAD   0x1F6
#define ATA_PRIMARY_COMM_REGSTAT 0x1F7
#define ATA_PRIMARY_ALTSTAT_DCR  0x3F6

// status flags
#define ATA_STAT_ERR  (1 << 0) 
#define ATA_STAT_DRQ  (1 << 3)
#define ATA_STAT_SRV  (1 << 4)
#define ATA_STAT_DF   (1 << 5) 
#define ATA_STAT_RDY  (1 << 6)
#define ATA_STAT_BSY  (1 << 7)

namespace HAL
{
    namespace Drivers
    { 
        void ATAController::Init()
        {
            SectorSize = 512;

            IDT::RegisterInterrupt(IRQ14, Callback);
            if (!Identify()) { Debug::Warning("No hard disk detected"); return; }

            Debug::OK("Initialized ATA driver");
        }

        void ATAController::Callback(Registers32* regs)
        {
            UNUSED(regs);
        }

        bool ATAController::Identify()
        {
            Ports::Read8(ATA_PRIMARY_COMM_REGSTAT);
            Ports::Write8(ATA_PRIMARY_DRIVE_HEAD, 0xA0);
            Ports::Read8(ATA_PRIMARY_COMM_REGSTAT);
            Ports::Write8(ATA_PRIMARY_SECCOUNT, 0);
            Ports::Read8(ATA_PRIMARY_COMM_REGSTAT);
            Ports::Write8(ATA_PRIMARY_LBA_LO, 0);
            Ports::Read8(ATA_PRIMARY_COMM_REGSTAT);
            Ports::Write8(ATA_PRIMARY_LBA_MID, 0);
            Ports::Read8(ATA_PRIMARY_COMM_REGSTAT);
            Ports::Write8(ATA_PRIMARY_LBA_HI, 0);
            Ports::Read8(ATA_PRIMARY_COMM_REGSTAT);
            Ports::Write8(ATA_PRIMARY_COMM_REGSTAT, 0xEC);
            Ports::Write8(ATA_PRIMARY_COMM_REGSTAT, 0xE7);

            // Read the status port. If it's zero, the drive does not exist.
            uint8_t status = Ports::Read8(ATA_PRIMARY_COMM_REGSTAT);

            while (status & ATA_STAT_BSY) 
            {
                uint32_t i = 0;
                while(1) { i++; }
                for(i = 0; i < 0x0FFFFFFF; i++) { }
                status = Ports::Read8(ATA_PRIMARY_COMM_REGSTAT);
            }
            
            if (status == 0) { asm volatile("sti"); return false; }

            while (status & ATA_STAT_BSY) { status = Ports::Read8(ATA_PRIMARY_COMM_REGSTAT); }

            uint8_t mid = Ports::Read8(ATA_PRIMARY_LBA_MID);
            uint8_t hi = Ports::Read8(ATA_PRIMARY_LBA_HI);
            if (mid || hi) { asm volatile("sti"); return false; }

            // Wait for ERR or DRQ
            while (!(status & (ATA_STAT_ERR | ATA_STAT_DRQ))) { status = Ports::Read8(ATA_PRIMARY_COMM_REGSTAT); }

            if (status & ATA_STAT_ERR) { asm volatile("sti"); return false; }

            uint8_t buff[256 * 2];
            Ports::ReadString(ATA_PRIMARY_DATA, buff, 256);
            return true;
        }

        bool ATAController::Read(uint64_t sector, uint32_t count, uint8_t* buffer)
        {
            Ports::Write8(ATA_PRIMARY_DRIVE_HEAD, 0x40);                        // Select master
            Ports::Write8(ATA_PRIMARY_SECCOUNT, (count >> 8) & 0xFF );          // sectorcount high
            Ports::Write8(ATA_PRIMARY_LBA_LO, (sector >> 24) & 0xFF);           // LBA4
            Ports::Write8(ATA_PRIMARY_LBA_MID, (sector >> 32) & 0xFF);          // LBA5
            Ports::Write8(ATA_PRIMARY_LBA_HI, (sector >> 40) & 0xFF);           // LBA6
            Ports::Write8(ATA_PRIMARY_SECCOUNT, count & 0xFF);                  // sectorcount low
            Ports::Write8(ATA_PRIMARY_LBA_LO, sector & 0xFF);                   // LBA1
            Ports::Write8(ATA_PRIMARY_LBA_MID, (sector >> 8) & 0xFF);           // LBA2
            Ports::Write8(ATA_PRIMARY_LBA_HI, (sector >> 16) & 0xFF);           // LBA3
            Ports::Write8(ATA_PRIMARY_COMM_REGSTAT, 0x24);                      // READ SECTORS EXT

            for (uint32_t i = 0; i < count; i++) 
            {
                while (true)
                {
                    uint8_t status = Ports::Read8(ATA_PRIMARY_COMM_REGSTAT);
                    if(status & ATA_STAT_DRQ) { break; }
                    if (status & ATA_STAT_ERR) { Debug::Error("Disk read error"); return false; }
                }
                Ports::ReadString(ATA_PRIMARY_DATA, (unsigned char *)buffer, 256);
                buffer += 256;
            }
            return true;
        }

        bool ATAController::Write(uint64_t sector, uint32_t count, uint8_t* buffer)
        {
            Ports::Write8(ATA_PRIMARY_DRIVE_HEAD, 0x40);                     // Select master
            Ports::Write8(ATA_PRIMARY_SECCOUNT, (count >> 8) & 0xFF );     // sectorcount high
            Ports::Write8(ATA_PRIMARY_LBA_LO, (sector >> 24) & 0xFF);           // LBA4
            Ports::Write8(ATA_PRIMARY_LBA_MID, (sector >> 32) & 0xFF);          // LBA5
            Ports::Write8(ATA_PRIMARY_LBA_HI, (sector >> 40) & 0xFF);           // LBA6
            Ports::Write8(ATA_PRIMARY_SECCOUNT, count & 0xFF);             // sectorcount low
            Ports::Write8(ATA_PRIMARY_LBA_LO, sector & 0xFF);                   // LBA1
            Ports::Write8(ATA_PRIMARY_LBA_MID, (sector >> 8) & 0xFF);           // LBA2
            Ports::Write8(ATA_PRIMARY_LBA_HI, (sector >> 16) & 0xFF);           // LBA3
            Ports::Write8(ATA_PRIMARY_COMM_REGSTAT, 0x34);                   // READ SECTORS EXT

            for (uint8_t i = 0; i < count; i++) 
            {
                while (true) 
                {
                    uint8_t status = Ports::Read8(ATA_PRIMARY_COMM_REGSTAT);
                    if(status & ATA_STAT_DRQ) { break; }
                    else if(status & ATA_STAT_ERR) { Debug::Error("Disk write error"); break; }
                }
                Ports::WriteString(ATA_PRIMARY_DATA, (unsigned char *)buffer, 256);
                buffer += 256;
            }

            // Flush the cache.
            Ports::Write8(ATA_PRIMARY_COMM_REGSTAT, 0xE7);
            // Poll for BSY.
            while (Ports::Read8(ATA_PRIMARY_COMM_REGSTAT) & ATA_STAT_BSY);
            return true;
        }

        bool ATAController::Fill(uint64_t sector, uint32_t count, uint8_t* buffer)
        {
             for (uint32_t i = 0; i < count; i++)
            {
                if (!Write(sector + i, 1, buffer + (SectorSize * i))) { return false; }
            }
            return true;
        }
    }
}