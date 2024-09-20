#ifndef _RLDRIVE_H_
#define _RLDRIVE_H_

#include "types.h"
#include "chrono/alarmclock/alarmclock.h"

#include <thread>
#include <mutex>
#include <condition_variable>

using std::thread;
using std::mutex;
using std::condition_variable;

// This class simulates the RL drive assembly.
class RlDrive
{
public:
    RlDrive ();
    ~RlDrive ();
    void startSeek (SimulatorClock::duration seekTime);
    void waitForDriveReady ();
    void driveThread ();

    // The drive status is temporarily public to allow access from
    // the unit and the command processor.
    // ToDo: Make driveStatus_ private.
    u16 driveStatus_ {0};

private:
    bool running_ {true};
    AlarmClock alarmClock_;

    SimulatorClock::duration seekTime_ {0};

    // Safe guard against drive access while a seek is in progress
    mutex driveMutex_;

    // Condition variable to wake up the seek timer
    condition_variable startSeek_;

    // Thread simulating drive timing
    thread driveThread_;
};

#endif // _RLDRIVE_H_