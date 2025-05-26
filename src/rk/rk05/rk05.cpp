#include "rk05.h"
#include "chrono/alarmclock/alarmclock.h"
#include "bitfield.h"

#include <memory>
#include <chrono>
#include <cstdlib>
#include <functional>

using std::shared_ptr;
using std::make_unique;
using std::chrono::seconds;
using std::abs;
using std::chrono::duration;
using std::bind;

using namespace RKTypes;

RK05::RK05 (Bus* bus, DriveInterface* controller, Window* window,
    shared_ptr<RK05Config> rk05Config)
    : 
    bus_ {bus},
    controller_ {controller}
{
    // In case the constructor is called in the unit tests, the window is
    // not available.
    if (window != nullptr)
        createBezel (window, rk05Config);

    // The maximum number of words that can be transferred in one read
    // or write command is 2^16
    buffer_ = make_unique<u16[]> (1 << 16);

    diskDrive_.attachFile (rk05Config->fileName, rk05Geometry,
        getAttachMode (rk05Config));

    // Initialize the drive status before an event is dispatched to the
    // state machine as the drive status is changed by the state machine
    // and the initialization would overwrite those changes.
    driveStatus_.value = +RKDS::SectorCounterEqualsSectorAddress {1} |
        +RKDS::ReadWriteSeekReady {1} |
        +RKDS::SectorCounterOK {1} |
        +RKDS::Rk05DiskOnLine {1} |
        +RKDS::DriveId {rk05Config->unitNumber};

    stateMachine_ = make_unique<StateMachine> (this,
        seconds (rk05Config->spinUpTime));

    running_ = true;
    driveThread_ = thread (&RK05::driveThread, this);

    // Immediately lock the drive on cylinder 0 if the spin up time is
    // zero.
    if (rk05Config->spinUpTime == 0)
        stateMachine_->dispatch (SpunUp {});
    else
        stateMachine_->dispatch (SpunDown {});
}

// Finish the drive thread
RK05::~RK05 ()
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

void RK05::runLoadSwitchClicked (Button::State state)
{
    if (get<Button::TwoPositionsState> (state) == Button::TwoPositionsState::On)
        sendTrigger (SpinUp {});
    else
        sendTrigger (SpinDown {});
}

// This function puts the specified event in the event queue thereby
// triggering the state machine to process the event.
void RK05::sendTrigger (Event event)
{
    unique_lock<mutex> lock {driveMutex_};
    eventQueue_.push (event);
    startCommand_.notify_one ();
}

void RK05::wtprotSwitchClicked (Button::State state)
{
}

// The drive is ready when it is in the LockedOn or Seeking state
bool RK05::isReady ()
{
    return stateMachine_->inState (LockedOn {}) ||
        stateMachine_->inState (Seeking {});
}

void RK05::seek (u16 cylinderAddress)
{
    sendTrigger (SeekCommand {seekTime (currentCylinderAddress_,
        cylinderAddress), bind (&RK05::seekCompleted, this)});

    // The current cylinder address actually should be set only when the
    // seek is completed, but as the seek cannot fail and the new cylinder
    // isn't available in the state machine transition we'll set it now.
    currentCylinderAddress_ = cylinderAddress;
}

// Head positioning performance specifcations:
//  10ms - for adjacent tracks,
//  50ms - average
//  85ms - for 200 track movement
// (EK-RK05-OP-001 p. 1-2)
//
// The formula 10ms + (numCylinders * 0.375) seems an acceptable approximation
// of the required seek time.
//
SimulatorClock::duration RK05::seekTime (u16 currentCylinderAddress,
    u16 newCylinderAddress)
{
    u16 numCylinders = abs (newCylinderAddress - currentCylinderAddress);
    return std::chrono::milliseconds (static_cast <uint64_t>
        (10 + (numCylinders * 0.375)));
}

void RK05::seekCompleted ()
{
    controller_->setDriveCondition (DriveCondition {driveStatus_,
        driveError_});
}

// ToDo: This function is a double with RL01_02::getAttachMode()
Bitmask<AttachFlags> RK05::getAttachMode (
    shared_ptr<RK05Config> rk05Config)
{
    Bitmask<AttachFlags> attachMode {AttachFlags::Default};

    if (rk05Config->writeProtect)
        attachMode |= AttachFlags::ReadOnly;
    if (rk05Config->newFile)
        attachMode |= AttachFlags::NewFile;
    if (rk05Config->overwrite)
        attachMode |= AttachFlags::Overwrite;

    return attachMode;
}