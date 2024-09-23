#ifndef _RLDRIVE_H_
#define _RLDRIVE_H_

#include "types.h"
#include "chrono/alarmclock/alarmclock.h"
#include "chrono/simulatorclock/simulatorclock.h"
#include "chrono/wakeupcall/wakeupcall.h"
#include "variantfsm/fsm.h"

#include <thread>
#include <mutex>
#include <condition_variable>
#include <queue>
#include <memory>

using std::thread;
using std::mutex;
using std::condition_variable;
using std::queue;
using std::unique_ptr;

// This class simulates the RL drive assembly. The class comprises two
// components: 
// - The controlling functions to be called from the RL01_02 unit,
// - The drive state machine which is executed in a seperate thread.
// 
// The controlling functions send command to the drive state machine thread
// via a ThreadSafeQueue.
//
class RlDrive
{
public:
    // Definition of the drive states
    struct LockOn {};       // The drive is locked on a cylinder
    struct Seeking {};      // The drive is seeking

    using State = std::variant <LockOn, Seeking>;

    // Definition of the drive events
    struct SeekCommand { SimulatorClock::duration seekTime; };
    struct TimeElapsed {};

    using Event = std::variant <SeekCommand, TimeElapsed>;

    RlDrive ();
    ~RlDrive ();
    void startSeek (SimulatorClock::duration seekTime);
    void waitForDriveReady ();

    // The drive status is temporarily public to allow access from
    // the unit and the command processor.
    // ToDo: Make driveStatus_ private.
    u16 driveStatus_ {0};

private:
    // Use the PIMPL idiom to be able to define the StateMachine outside
    // of the RlDrive class
    class StateMachine;
    unique_ptr<StateMachine> stateMachine_;

    bool running_ {true};
    AlarmClock alarmClock_;

    // Safe guard against drive access while a seek is in progress
    mutex driveMutex_;

    // Condition variable to wake up the seek timer
    condition_variable startCommand_;

    // Thread simulating drive timing
    thread driveThread_;

    // The queue by which commands and timeElapsed events are sent to
    // the drive thread.
    queue<Event> eventQueue_;

    void driveThread ();
};

class RlDrive::StateMachine : 
    public variantFsm::Fsm<StateMachine, Event, State>,
    public WakeUpCall
{
public:
    StateMachine (RlDrive* context);

    State transition (LockOn&&, SeekCommand);   // -> Seeking
    State transition (Seeking&&, TimeElapsed);  // -> LockOn

    // Define the default transition for transitions not explicitly
    // defined above. In these case the event is ignored.
    template <typename S, typename E>
    State transition (S&& state, E)
    {
        return state;
    }

    // Functions required for the wakeupCall interface. waitFor() and
    // id() are not used for the wakeup calls in the state machine.
    void ring (uint64_t currentTime);
    void waitFor () {}
    size_t id () { return 0; }

private:
    RlDrive* context_ {nullptr};
};

#endif // _RLDRIVE_H_