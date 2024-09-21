#ifndef _RLDRIVE_H_
#define _RLDRIVE_H_

#include "types.h"
#include "chrono/alarmclock/alarmclock.h"
#include "variantfsm/fsm.h"

#include <thread>
#include <mutex>
#include <condition_variable>
#include <queue>

using std::thread;
using std::mutex;
using std::condition_variable;
using std::queue;

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
    // Definition of the drive commands
    struct SeekCommand { SimulatorClock::duration seekTime; };

    using DriveCommand = std::variant <SeekCommand>;

    RlDrive ();
    ~RlDrive ();
    void execute (DriveCommand command);
    void waitForDriveReady ();

    // The drive status is temporarily public to allow access from
    // the unit and the command processor.
    // ToDo: Make driveStatus_ private.
    u16 driveStatus_ {0};

private:
    bool running_ {true};
    AlarmClock alarmClock_;

    // Safe guard against drive access while a seek is in progress
    mutex driveMutex_;

    // Condition variable to wake up the seek timer
    condition_variable startCommand_;

    // Thread simulating drive timing
    thread driveThread_;

    // The queue by which commands are sent to the drive thread
    queue<DriveCommand> commandQueue_;

    void driveThread ();
};

#endif // _RLDRIVE_H_