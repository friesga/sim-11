#ifndef _RK11DREGISTERS_H_
#define _RK11DREGISTERS_H_

// Definition of the RK11-D register bit assignments

union RKDS
{
    u16 value;
    BitField<u16, 0, 3> sectorCounter;
    BitField<u16, 4> sectorCounterEqualsSectorAddress;
    BitField<u16, 5> writeProtectStatus;
    BitField<u16, 6> readWriteSeekReady;
    BitField<u16, 7> driveReady;
    BitField<u16, 8> sectorCounterOK;
    BitField<u16, 9> seekIncomplete;
    BitField<u16, 10> driveUnsafe;
    BitField<u16, 11> rk05DiskOnLine;
    BitField<u16, 12> drivePowerLow;
    BitField<u16, 13, 15> driveId;
};

// Error register. This is a read-only register.
union rker
{
    u16 value;
    BitField<u16, 0> writeCheckError;
    BitField<u16, 1> checksumError;
    BitField<u16, 2, 4> unused;
    BitField<u16, 5> nonexistentSector;
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
union rkcs
{
    u16 value;
    BitField<u16, 0> go;                        // Write Only
    BitField<u16, 1, 3> function;               // Read/Write
    BitField<u16, 4, 5> memoryExtension;        // Read/Write
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
union rkda
{
    u16 value;
    BitField<u16, 0, 3> sectorAddress;
    BitField<u16, 4> surface;
    BitField<u16, 5, 12> cylinderAddress;
    BitField<u16, 13, 15> driveSelect;
};


#endif // _RK11DREGISTERS_H_