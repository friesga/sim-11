#ifndef _RL012_H_
#define _RL012_H_

#include "unit/unit.h"
#include "types.h"
#include "statuscodes.h"

#include <mutex>
#include <thread>
#include <condition_variable>

using std::shared_ptr;


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
class RL01_2 : public Unit
{
    // All RLV12Commands need access to the file pointer and unit status
    friend class RLV12;
    friend class CmdProcessor;

    int32_t currentDiskAddress_;
    Bitmask<RlStatus> rlStatus_;
    int32_t driveStatus_;
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
    RL01_2 (PDP11Peripheral *owningDevice);
    ~RL01_2 ();

    // Seek timer, started in a separate thread
    void seekTimer ();

    // Required functions
    StatusCode configure (shared_ptr<RLUnitConfig> rlUnitConfig);
};

#endif // _RL012_H_