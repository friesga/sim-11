#include "rl01_02.h"
#include "../rlv12.h"

#include <chrono>

using std::chrono::seconds;

using std::make_unique;
using namespace std::chrono_literals;

// Constructor
// By default the unit is off-line. It is set on-line when a file is
// attached.
RL01_02::RL01_02 (PDP11Peripheral *owningDevice)
    :
    Unit (owningDevice),
    currentDiskAddress_ {0},
    rlStatus_ {},
    seekTime_ {}
{}

// Finish the drive thread
RL01_02::~RL01_02 ()
{
    // Stop a potentionally running drive thread
    if (running_)
    {
        // Wakeup the drive thread with the indication to finish
        running_ = false;
        startCommand_.notify_one ();

        // Wait till the thread has finished
        driveThread_.join ();
    }
}

StatusCode RL01_02::init (shared_ptr<RLUnitConfig> rlUnitConfig,
    Window* window)
{
    createBezel (window, rlUnitConfig);

    return init (rlUnitConfig);
}

// This version of the init function doesn't create a bezel and is meant
// to be called by the unit tests.
StatusCode RL01_02::init (shared_ptr<RLUnitConfig> rlUnitConfig)
{
    if (StatusCode status = configure (rlUnitConfig); status != StatusCode::OK)
        return status;

    running_ = true;
    driveThread_ = std::thread (&RL01_02::driveThread, this);
    stateMachine_ = make_unique<StateMachine> (this, 
        seconds (rlUnitConfig->spinUpTime));

    // Immediataely lock the drive on cylinder 0 if the spin up time is
    // zero.
    if (rlUnitConfig->spinUpTime == 0)
        stateMachine_->dispatch (SpunUp {});
    else
        stateMachine_->dispatch (SpunDown {});

    return StatusCode::OK;
}

// Reset errors
// According to Table 4-6 in EK-RL012-UG-005 this also resets the
// Volume Check condition
void RL01_02::resetDriveError ()
{
    driveStatus_ &= ~(RLV12const::MPR_GS_AnyError |
        RLV12const::MPR_GS_VolumeCheck);
}

// This function returns the drive status as expected in the MPR on a
// Get Status command.
// 
// The MPR contains information from several sources:
// - State information from the drive (bits 0-5),
// - Information resulting from the last access (Head Select),
// - Configuration information (Drive Type),
// - Error conditions.
//
// These statuses are set at the location at which the status changes,
// except for the head select bit. That bit is derived from the current
// disk address which changes often.
//
u16 RL01_02::driveStatus ()
{
    return driveStatus_ | (currentDiskAddress_ & RLV12const::MPR_GS_HeadSelect);
}

// This function puts the specified event in the event queue thereby
// triggering the state machine to process the event.
void RL01_02::sendTrigger (Event event)
{
    unique_lock<mutex> lock {driveMutex_};
    eventQueue_.push (event);
    startCommand_.notify_one ();
}

// Calculate the position of a sector as an offset in the file from
// the specified diskAddress
int32_t RL01_02::filePosition (int32_t diskAddress) const
{
    return (RLV12const::getTrack (diskAddress) * RLV12const::sectorsPerSurface +
        RLV12const::getSector (diskAddress)) * RLV12const::wordsPerSector *
        sizeof (int16_t);
}

void RL01_02::waitForDriveReady ()
{
    std::lock_guard<std::mutex> guard {driveMutex_};
}