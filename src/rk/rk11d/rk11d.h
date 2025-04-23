#ifndef _RK11D_H_
#define _RK11D_H_

#include "pdp11peripheral/pdp11peripheral.h"
#include "configdata/rk/rk11d/rk11dconfig/rk11dconfig.h"
#include "rk/rk05/rk05.h"
#include "panel.h"
#include "bitfield.h"

#include <vector>
#include <string>

using std::vector;
using std::unique_ptr;

class RK11D : public PDP11Peripheral
{
private:
    // Define RK11-D registers as offsets from the controller's base address
    //
    // Address 777414 is unused.
    enum
    {
        RKDS = 00,      // Drive Status register
        RKER = 02,      // Error register
        RKCS = 04,      // Control Status register
        RKWC = 06,      // Word Count register
        RKBA = 010,     // Current Bus Address register
        RKDA = 012,     // Disk Address register
        RKDB = 016,     // Data Buffer register
    };

    // Definition of the controller's registers
    //
    // Drive Status register. This is a read-only register and contains the
    // status of the selected drive and current sector address.
    //
    union rkds
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
    }
    rkds_ {0};

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
    }
    rker_ {0};

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
    }
    rkcs_ {0};

    // Word Count register. This is a read/write register. 
    u16 rkwc_ {0};

    // Current Bus Address register. This is a read/write register.
    u16 rkba_ {0};

    // Disk Address register. This is a read/write register.
    union rkda
    {
        u16 value;
        BitField<u16, 0, 3> sectorAddress;
        BitField<u16, 4> surface;
        BitField<u16, 5, 12> cylinderAddress;
        BitField<u16, 13, 15> driveSelect;
    }
    rkda_ {0};

    // Data Buffer register. This is a read/write register.
    u16 rkdb_ {0};

    // Definition of (pointers to) the attached RK05 drives
    vector<unique_ptr<RK05>> rk05Drives_ {};

public:
    RK11D (Bus* bus, Window* window, shared_ptr<RK11DConfig> rk11dConfig);
	CondData<u16> read (BusAddress busAddress) override;
	StatusCode writeWord (BusAddress busAddress, u16 value) override;
	bool responsible (BusAddress busAddress) override;
	void reset () override;


};


#endif // _RK11D_H_