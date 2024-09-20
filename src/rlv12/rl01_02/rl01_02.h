#ifndef _RL01_02_H_
#define _RL01_02_H_

#include "unit/unit.h"
#include "types.h"
#include "statuscodes.h"
#include "chrono/alarmclock/alarmclock.h"
#include "../rlv12command/rlv12command.h"
#include "rldrive/rldrive.h"

#include <mutex>
#include <thread>
#include <condition_variable>
#include <utility>

using std::shared_ptr;
using std::pair;


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

// RLO1/RL02 unit
class RL01_02 : public Unit
{
public:
    // Definition for the procedures to calculate the new head position
    enum class HeadPositionProcedure
    {
        Increment,
        Rotate,
        DiskAddressRegister
    };

    // Constructor and destructor
    RL01_02 (PDP11Peripheral* owningDevice);
    ~RL01_02 ();

    // Required functions
    StatusCode configure (shared_ptr<RLUnitConfig> rlUnitConfig);

    // Seek timer, started in a separate thread
    void seekTimer ();

    bool available ();
    pair<bool, size_t> readData (RLV12Command& rlv12Command, u16* buffer,
        HeadPositionProcedure procedure, u16 diskAddressRegister);
    pair<bool, size_t> writeData (RLV12Command& rlv12Command, u16* buffer,
        size_t numWords, HeadPositionProcedure procedure, u16 diskAddressRegister);
    u16 readHeader (RLV12Command& rlv12Command, HeadPositionProcedure procedure,
        u16 diskAddressRegister);
    void seek (u16 diskAddressRegister);

private:
    // All RLV12Commands need access to the file pointer and unit status
    friend class RLV12;
    friend class CmdProcessor;

    // The drive needs access to the drive status. This will be removed
    // on implementation of the drive state machine.
    friend class RlDrive;

    int32_t currentDiskAddress_;
    Bitmask<RlStatus> rlStatus_;
    AlarmClock alarmClock_;

    // Calculated seek time
    SimulatorClock::duration seekTime_;

    int32_t filePosition (int32_t diskAddress) const;
    void updateHeadPosition (HeadPositionProcedure procedure,
        s32 wordCount, u16 diskAddressRegister);

    // The drive for this unit
    RlDrive drive_ {};
};

#endif // _RL01_02_H_