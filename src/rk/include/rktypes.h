#ifndef _RKTYPES_H_
#define _RKTYPES_H_

#include "bitfield.h"
#include "busaddress/busaddress.h"
#include "concepts/geometry/geometry.h"

#include <variant>

using std::variant;

namespace RKTypes
{
    // Definition of the RK05 drive format:
    // - 12 sectors/track
    // - 2 disk surfaces/disk
    // - 203 cylinders/disk drive
    // - 256 words/sector
    u16 const SectorsPerSurface = 12;
    u16 const NumberOfHeads = 2;
    u16 const CylindersPerDisk = 203;
    u16 const WordsPerSector = 256;

    // Definition of the RK05 drive format
    Geometry const rk05Geometry_ {RKTypes::SectorsPerSurface, RKTypes::NumberOfHeads,
        RKTypes::CylindersPerDisk, RKTypes::WordsPerSector};

    // Definition of the RK11-D register bit assignments
    // 
    // Drive Status register. This is a read-only register and contains the
    // status of the selected drive and current sector address.
    //
    union RKDS
    {
        using SectorCounter = BitField<u16, 0, 4>;
        using SectorCounterEqualsSectorAddress = BitField<u16, 4>;
        using WriteProtectStatus = BitField<u16, 5>;
        using ReadWriteSeekReady = BitField<u16, 6>;
        using DriveReady = BitField<u16, 7>;
        using SectorCounterOK = BitField<u16, 8>;
        using SeekIncomplete = BitField<u16, 9>;
        using DriveUnsafe = BitField<u16, 10>;
        using Rk05DiskOnLine = BitField<u16, 11>;
        using DrivePowerLow = BitField<u16, 12>;
        using DriveId = BitField<u16, 13, 15>;

        u16 value;
        SectorCounter sectorCounter;
        SectorCounterEqualsSectorAddress sectorCounterEqualsSectorAddress;
        WriteProtectStatus writeProtectStatus;
        ReadWriteSeekReady readWriteSeekReady;
        DriveReady driveReady;
        SectorCounterOK sectorCounterOK;
        SeekIncomplete seekIncomplete;
        DriveUnsafe driveUnsafe;
        Rk05DiskOnLine rk05DiskOnLine;
        DrivePowerLow drivePowerLow;
        DriveId driveId;
    };

    // Error register. This is a read-only register.
    union RKER
    {
        u16 value;
        BitField<u16, 0> writeCheckError;
        BitField<u16, 1> checksumError;
        BitField<u16, 2, 3> unused;
        BitField<u16, 5> nonexistentSector;
        BitField<u16, 5, 11> hardError;
        BitField<u16, 6> nonexistentCylinder;
        BitField<u16, 7> nonExistentDisk;
        BitField<u16, 8> timingError;
        BitField<u16, 9> dateLate;
        BitField<u16, 10> nonexistentMemory;
        BitField<u16, 11> programmingError;
        BitField<u16, 12> seekError;
        BitField<u16, 13> writeLockoutViolation;
        BitField<u16, 14> overrun;
        BitField<u16, 15> driveError;
    };

    // Constrol Status register.
    union RKCS
    {
        u16 value;
        BitField<u16, 0> go;                        // Write Only
        BitField<u16, 1, 3> operation;              // Read/Write
        BitField<u16, 4, 2> memoryExtension;        // Read/Write
        BitField<u16, 6> interruptOnDoneEnable;     // Read/Write
        BitField<u16, 7> controlReady;              // Read Only
        BitField<u16, 8> stopOnSoftError;           // Read/Write
        BitField<u16, 9> extraBit;                  // Unused for RK11-D/E
        BitField<u16, 10> format;                   // Read/Write
        BitField<u16, 11> inhibitIncrementingRKBA;  // Read/Write
        BitField<u16, 12> unused;
        BitField<u16, 13> searchComplete;           // Read Only
        BitField<u16, 14> hardError;                // Read Only
        BitField<u16, 15> error;                    // Read Only
    };

    // Disk Address register. This is a read/write register.
    // 
    // The RKDA is incremented automatically at the end of each disk sector
    // (EK-RK11D-MM-002, p. 3-8). To this end the register is equipped with
    // the assignment and addition operators. The argument for the addition
    // operators is the numbers of bytes (instead of number of sectors as
    // might be expected). This ensures the RK11D doesn't require knowledge
    // of the RK05 geometry.
    //
    union RKDA
    {
        u16 value;
        BitField<u16, 0, 4> sectorAddress;
        BitField<u16, 4> surface;
        BitField<u16, 5, 8> cylinderAddress;
        BitField<u16, 13, 3> driveSelect;

        RKDA& operator= (DiskAddress const& da)
        {
            sectorAddress = da.sector;
            surface = da.head;
            cylinderAddress = da.cylinder;
            this->driveSelect = driveSelect;
            return *this;
        }

        RKDA operator+ (u16 numWords)
        {
            u32 lbn = rk05Geometry_.LBN (DiskAddress {sectorAddress, surface,
                cylinderAddress});
            *this = rk05Geometry_.lbnTodiskAddress (lbn +
                numWords / rk05Geometry_.wordsPerSector ());
            return *this;
        }

        RKDA& operator+= (u16 numWords)
        {
            *this = *this + numWords;
            return *this;
        }
    };

    // Definition of the RK11-D Operations
    // 
    // Disclaimer: use of an enum instead of an enum class as the enum class
    // makes comparing with u16 values cumbersome, even if the enum class is
    // derived from u16.
    //
    enum Operation
    {
        ControlReset,
        Write,
        Read,
        WriteCheck,
        Seek,
        ReadCheck,
        DriveReset,
        WriteLock
    };

    // Definition of the function to be processed by the RK11D action processor
    struct Function
    {
        RKCS rkcs;
        RKDA diskAddress;
        u16 wordCount;
        BusAddress busAddress;
    };

    // Definition of a seek complete report by a RK05 drive
    struct SeekCompleteReport
    {
        u16 driveId;
        RKER rker;
    };
};

#endif // _RKTYPES_H_
