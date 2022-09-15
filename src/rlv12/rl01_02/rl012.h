#ifndef _RL012_H_
#define _RL012_H_

#include "unit/unit.h"
#include "types.h"
#include "statuscodes.h"
#include "configdata/include/configdata.h"

// RL01/02 unit status flags. These flags are used in the definition of 
// Bitmask<RlStatus> and provide a compile-time type safety for the use
// of these flags.
// The flags are used for configuration and/or run-time status. This cannot
// be separated easily as some configuration flags (e.g. UNIT_RL02) are
// updated run-time to reflect the actual situation.
//
// ToDo: Clean up unused flags
//
enum class RlStatus
{
    UNIT_WLK,           // hwre write lock 
    UNIT_RL02,          // RL01 vs RL02 
    UNIT_AUTO,          // autosize enable 
    UNIT_DUMMY,         // dummy flag, for SET BADBLOCK 
    UNIT_OFFL,          // unit off line 
    UNIT_BRUSH,         // unit has brushes 
    _                   // Required for Bitmask
};

// RLDS 
// NI = not implemented,
// * = kept in driveStatus_,
// ^ = kept in currentTrack_,
// ! = kept in uptr

#define RLDS_M_STATE    (07)
#define RLDS_LOAD       (0)                             // no cartridge 
#define RLDS_SPIN       (1)                             // spin-up 
#define RLDS_BRUSH      (2)                             // brush cycle *! 
#define RLDS_HLOAD      (3)                             // load heads 
#define RLDS_SEEK       (4)                             // drive seeking * 
#define RLDS_LOCK       (5)                             // lock on * 
#define RLDS_UNL        (6)                             // unload heads 
#define RLDS_DOWN       (7)                             // spin-down 
#define RLDS_BHO        (0000010)                       // brushes home * 
#define RLDS_HDO        (0000020)                       // heads out * 
#define RLDS_CVO        (0000040)                       // cover open * 
#define RLDS_HD         (0000100)                       // head select ^ 
#define RLDS_RL02       (0000200)                       // RL02 ! 
#define RLDS_DSE        (0000400)                       // drv sel err 
#define RLDS_VCK        (0001000)                       // vol check * 
#define RLDS_WGE        (0002000)                       // wr gate err * 
#define RLDS_SPE        (0004000)                       // spin err * 
#define RLDS_STO        (0010000)                       // seek time out * 
#define RLDS_WLK        (0020000)                       // wr locked ! 
#define RLDS_HCE        (0040000)                       // hd curr err NI 
#define RLDS_WDE        (0100000)                       // wr data err NI 
#define RLDS_ERR        (RLDS_WDE|RLDS_HCE|RLDS_STO|RLDS_SPE|RLDS_WGE| \
                         RLDS_VCK|RLDS_DSE)             // errors bits 

// RLO1/RL02 unit
class RL01_2 : public Unit
{
    // All RLV12Commands need access to the file pointer and unit status
    friend class RLV12;
    friend class RLV12Command;
    friend class RLV12MaintenanceCmd;
    friend class RLV12ReadCmd;
    friend class RLV12ReadHeaderCmd;
    friend class RLV12ReadNoHeaderCmd;
    friend class RLV12SeekCmd;
    friend class RLV12WriteCmd;
    friend class RLV12WriteCheckCmd;

    // ToDo: Split currentDiskAddress_ in three separate variables
    int32_t currentDiskAddress_;
    Bitmask<RlStatus> rlStatus_;
    int32_t driveStatus_;
    int32_t function_;

public:
    // Constructor
    RL01_2 (BusDevice *owningDevice);

    // Required functions
    StatusCode configure (UnitConfig &unitConfig) override;

    // Make the class a function object
    void operator() ();
};


#endif // _RL012_H_