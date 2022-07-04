#ifndef _RLV12_H_
#define _RLV12_H_

#include "device.h"
#include "rl012.h"
#include "qbus/qbus.h"

#include <array>
#include <memory>       // for std::shared_ptr<>

// From simh pdp11_defs.h
#define IOPAGEBASE      017760000                       // 2**22 - 2**13
#define IOBA_RL         (IOPAGEBASE + 014400)           // RL11
#define VEC_RL          0160

// RLV12 constants
// N.B. RL_NUMCY is the value for a RL01; a RL02 contains 512 cylinders per
// cartridge.
// ToDo: Replace definitions by constants (or enums)
#define RL_NUMWD        (128)                           // Words/sector
#define RL_NUMSC        (40)                            // Sectors/surface
#define RL_NUMSF        (2)                             // Surfaces/cylinder
#define RL_NUMCY        (256)                           // Cylinders/cartridge
#define RL_NUMDRIVES    (4)                             // Drives/controller
#define RL_MAXFR        (RL_NUMSC * RL_NUMWD)           // Max transfer
#define RL01_SIZE (RL_NUMCY * RL_NUMSF * RL_NUMSC * RL_NUMWD) // Words/cartridge
#define RL02_SIZE       (RL01_SIZE * 2)                 // Words/cartridge


// RLV12 controller
class RLV12 : public Device, QBUSModule
{
    // Define controller registers
    u16 rlcs;       // Control/status register
    u16 rlba;       // Bus Address register
    u16 rlda;       // Disk address register
    u16 rlmpr;      // Multi purpose regsister
    u16 rlbae;      // Bus Address Extension register

    // Define transfer buffer
    u16 *rlxb_;
    
    // A RLV12 can have a maximum of four units
    std::array<RL01_2, RL_NUMDRIVES> units_
    {
        RL01_2 (this),
        RL01_2 (this),
        RL01_2 (this),
        RL01_2 (this)
    };

public:
    // Constructor/destructor
    RLV12 ();
    ~RLV12 ();

    // Required functions
    void reset () override;
};


#endif _RLV12_H_