#include "rl01_02.h"
#include "../rlv12.h"

#include <chrono>

using std::chrono::seconds;
using std::make_unique;
using namespace std::chrono_literals;

// Constructor
// By default the unit is off-line. It is set on-line when a file is
// attached.
RL01_02::RL01_02 (AbstractBusDevice* owningDevice)
    :
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
    if (StatusCode status = configure (rlUnitConfig); status != StatusCode::Success)
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

    return StatusCode::Success;
}

// Reset errors and clear the FAULT indicator
// According to Table 4-6 in EK-RL012-UG-005 this also resets the
// Volume Check condition
void RL01_02::resetDriveError ()
{
    faultIndicator_->show (Indicator::State::Off);

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

bool RL01_02::volumeCheck () const
{
    return driveStatus_ & RLV12const::MPR_GS_VolumeCheck;
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

// The variable seeksInProgress_ can have three values:
// - 0: No seeks are in progress. This is the default value and the value
//      the variable is set to when a seek is completed.
// - 1: A Seek Command has been issued. In RLV12::writeWord() clearDeviceReady()
//      has been called to immediately report the drive no longer ready.
//      RL01_02::seek() calls waitForSeekComplete() which falls through as the
//      seek is not yet in progress.
// - 2: A Seek Command has been issued while another seek is in progress.
//      RL01_02::seek() has to wait till the first seek has been completed by
//      calling waitForSeekComplete().
//
// This implies that the drive is ready when there is no seek in progress, i.e.
// seeksInProgress_ equals zero.
//
void RL01_02::setDriveReady ()
{
    std::lock_guard<std::mutex> guard {driveReadyMutex_};
    seeksInProgress_ = 0;

    // If a thread is waiting wake up this thread
    driveReadyCondition_.notify_one ();
}

void RL01_02::clearDriveReady ()
{
    std::lock_guard<std::mutex> guard {driveReadyMutex_};
    ++seeksInProgress_;
}

// In principle the drive is ready when it is in the LockedOn state. That is
// not a sufficient condition however as the drive isn't ready anymore when
// a seek is initiated (in RLV12::writeWord()). At that point clearDriveReady()
// is called.
bool RL01_02::driveReady ()
{
    std::lock_guard<std::mutex> guard {driveReadyMutex_};
    return stateMachine_->inState (LockedOn {}) && seeksInProgress_ == 0;
}

void RL01_02::waitForDriveReady ()
{
    // Lock the drive ready mutex
    unique_lock<std::mutex> lock (driveReadyMutex_);

    // wait() calls unlock() on the given lock and blocks the thread.
    // The thread will be unblocked when setDriveReady() is executed.
    // As it may also be unblocked spuriously, seeksInProgress_ has to be
    // tested on wakeup. When the thread is unblocked, it locks the given
    // lock so we have to unlock it on wakeup.
    if (seeksInProgress_ > 0)
        driveReadyCondition_.wait (lock, [this] { return seeksInProgress_ == 0; });

    lock.unlock ();
}

// In situations in which the drive is not ready and a seek command is
// initiated in RLV12::writeWord(), the seek is aborted in
// CmdProcessor::seekCmd(). In that case the number of seeks in progress
// has to be reset.
void RL01_02::setSeekIncomplete ()
{
    // Lock the drive ready mutex
    unique_lock<std::mutex> lock (driveReadyMutex_);
    seeksInProgress_ = 0;
}

void RL01_02::waitForSeekComplete ()
{
    // Lock the drive ready mutex
    unique_lock<std::mutex> lock (driveReadyMutex_);

    // See comment in waitForDriveReady()
    if (seeksInProgress_ > 1)
        driveReadyCondition_.wait (lock, [this] { return seeksInProgress_ == 0; });

    lock.unlock ();
}
