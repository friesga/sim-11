#ifndef _RL012_H_
#define _RL012_H_

#include "unit/unit.h"
#include "types.h"
#include "statuscodes.h"
#include "configdata/include/configdata.h"

#include <mutex>

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
// The RLDS bits correspond with the status as returned in the MPR
// after a Get Status command.
// 
// ToDo: Move RLDS bit definitions to rlv12.h
//
// These bits define the state of the drive (STA, STB and STC).
#define RLDS_M_STATE    (07)
#define RLDS_LOAD       (0)                             // no cartridge 
#define RLDS_SPIN       (1)                             // spin-up 
#define RLDS_BRUSH      (2)                             // brush cycle *! 
#define RLDS_HLOAD      (3)                             // load heads 
#define RLDS_SEEK       (4)                             // drive seeking * 
#define RLDS_LOCK       (5)                             // lock on * 
#define RLDS_UNL        (6)                             // unload heads 
#define RLDS_DOWN       (7)                             // spin-down 
// Definition of MPR bits 3:15
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
#define RLDS_ERR        (RLDS_WDE|RLDS_HCE|RLDS_WLK| \
                         RLDS_STO|RLDS_SPE|RLDS_WGE| \
                         RLDS_VCK|RLDS_DSE)             // Error bits

// DR RDY will be negated [i.e. cleared] when a drive error
// occurs except when an attempt has been made to write on a
// write-protected drive or if volume check is set.
// (EK-RLV12-TD-001, p3-7)
#define RLDS_DRRDY_NEGATE (RLDS_WDE|RLDS_HCE|RLDS_STO| \
                           RLDS_SPE|RLDS_WGE|RLDS_DSE)

// RLO1/RL02 unit
class RL01_2 : public Unit
{
    // All RLV12Commands need access to the file pointer and unit status
    friend class RLV12;
    friend class CmdProcessor;
    friend class RLV12Command;
    friend class RLV12GetStatusCmd;
    friend class RLV12MaintenanceCmd;
    friend class RLV12ReadCmd;
    friend class RLV12ReadHeaderCmd;
    friend class RLV12ReadNoHeaderCmd;
    friend class RLV12SeekCmd;
    friend class RLV12WriteCmd;
    friend class RLV12WriteCheckCmd;

    int32_t currentDiskAddress_;
    Bitmask<RlStatus> rlStatus_;
    int32_t driveStatus_;
    int32_t function_;
    bool running_;

    // Thread simulating seek timing
    std::thread seekTimerThread_;

    // Safe guard against drive access while a seek is in progress
    std::mutex driveMutex_;

    // Condition variable to wake up the seek timer
    std::condition_variable startSeek_;

    // Calculated seek time
    int32_t seekTime_;

public:
    // Constructor and destructor
    RL01_2 (BusDevice *owningDevice);
    ~RL01_2 ();

    // Seek timer, started in a separate thread
    void seekTimer ();

    // Required functions
    StatusCode configure (UnitConfig &unitConfig) override;
};

#endif // _RL012_H_