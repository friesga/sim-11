#include "rl01_02.h"
#include "../rlv12.h"

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
    Window* window, shared_ptr<Cabinet::Position> cabinetPosition)
{
    if (configure (rlUnitConfig) != StatusCode::OK)
        return StatusCode::ArgumentError;

    createBezel (window, rlUnitConfig);

    running_ = true;
    driveThread_ = std::thread (&RL01_02::driveThread, this);
    stateMachine_ = make_unique<StateMachine> (this, 0s);

    // Perform a transition from the initial state to the LockOn state.
    unique_lock<mutex> lock {driveMutex_};
    eventQueue_.push (SpinUpTime0 {});
    startCommand_.notify_one ();

    return StatusCode::OK;
}

// This version of the init function doesn't create a bezel and is meant
// to be called by the unit tests.
StatusCode RL01_02::init (shared_ptr<RLUnitConfig> rlUnitConfig)
{
    if (StatusCode status = configure (rlUnitConfig); status != StatusCode::OK)
        return status;

    running_ = true;
    driveThread_ = std::thread (&RL01_02::driveThread, this);
    stateMachine_ = make_unique<StateMachine> (this, 0s);

    // Perform a transition from the initial state to the LockOn state.
    unique_lock<mutex> lock {driveMutex_};
    eventQueue_.push (SpinUpTime0 {});
    startCommand_.notify_one ();

    return StatusCode::OK;
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