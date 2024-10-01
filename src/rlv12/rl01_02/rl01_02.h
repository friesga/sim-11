#ifndef _RL01_02_H_
#define _RL01_02_H_

#include "unit/unit.h"
#include "types.h"
#include "statuscodes.h"
#include "chrono/alarmclock/alarmclock.h"
#include "variantfsm/fsm.h"
#include "rlv12/rlv12command/rlv12command.h"
#include "panel.h"

#include <mutex>
#include <thread>
#include <condition_variable>
#include <utility>
#include <queue>
#include <memory>

using std::thread;
using std::mutex;
using std::condition_variable;
using std::queue;
using std::unique_ptr;
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

    bool available ();
    pair<bool, size_t> readData (RLV12Command& rlv12Command, u16* buffer,
        HeadPositionProcedure procedure, u16 diskAddressRegister);
    pair<bool, size_t> writeData (RLV12Command& rlv12Command, u16* buffer,
        size_t numWords, HeadPositionProcedure procedure, u16 diskAddressRegister);
    u16 readHeader (RLV12Command& rlv12Command, HeadPositionProcedure procedure,
        u16 diskAddressRegister);
    void seek (u16 diskAddressRegister);

    void waitForDriveReady ();

    StatusCode init (shared_ptr<RLUnitConfig> rlUnitConfig,
        Window* window, shared_ptr<Cabinet::Position> cabinetPosition);

private:
    // Definition of the drive states
    struct Initial {};      // State machine initial state
    struct SpinningUp {};
    struct LockOn {};       // The drive is locked on a cylinder
    struct Seeking {};      // The drive is seeking

    using State = std::variant <Initial, SpinningUp, LockOn, Seeking>;

    // Definition of the drive events
    struct SpinUpTime0 {};
    struct SpinUpTimeX {};
    struct SeekCommand { SimulatorClock::duration seekTime; };
    struct TimeElapsed {};

    using Event = std::variant <SpinUpTime0, SpinUpTimeX, SeekCommand, TimeElapsed>;

    // All RLV12Commands need access to the file pointer and unit status
    friend class RLV12;
    friend class CmdProcessor;
    
    // Use the PIMPL idiom to be able to define the StateMachine outside
    // of the RlDrive class
    class StateMachine;
    unique_ptr<StateMachine> stateMachine_;

    int32_t currentDiskAddress_;
    Bitmask<RlStatus> rlStatus_;
    u16 driveStatus_ {0};
    bool running_ {false};
    AlarmClock alarmClock_;
    SimulatorClock::duration spinUpTime_ {0};

    // Calculated seek time
    SimulatorClock::duration seekTime_;

    // Safe guard against drive access while a seek is in progress
    mutex driveMutex_;

    // Condition variable to wake up the seek timer
    condition_variable startCommand_;

    // Thread simulating drive timing
    thread driveThread_;

    // The queue by which commands and timeElapsed events are sent to
    // the drive thread.
    queue<Event> eventQueue_;

    // The drive thread for this unit
    void driveThread ();

    // Buttons and indicators
    Indicator* readyIndicator_ {nullptr};

    // Button and indicators positions and dimensions
    Frame<float> loadButtonFrame     {0.703, 0.538, 0.030, 0.060};
    Frame<float> readyIndicatorFrame {0.743, 0.538, 0.030, 0.060};

    StatusCode configure (shared_ptr<RLUnitConfig> rlUnitConfig);
    void createBezel (Window* window, shared_ptr<Cabinet::Position> cabinetPosition);
    int32_t filePosition (int32_t diskAddress) const;
    void updateHeadPosition (HeadPositionProcedure procedure,
        s32 wordCount, u16 diskAddressRegister);

    void loadButtonClicked (Button::State state);
};

// Definition of the state machine for the drive. The class has to be defined
// in the same compilation unit to prevent incomplete type compilation errors.
class RL01_02::StateMachine :
    public variantFsm::Fsm<StateMachine, Event, State>,
    public WakeUpCall
{
public:
    StateMachine (RL01_02* context, SimulatorClock::duration spinUpTime);

    State transition (Initial&&, SpinUpTime0);  // -> LockOn
    void entry (LockOn);
    State transition (LockOn&&, SeekCommand);   // -> Seeking
    void exit (LockOn);
    State transition (Seeking&&, TimeElapsed);  // -> LockOn

    // Define the default transition for transitions not explicitly
    // defined above. In these case the event is ignored.
    template <typename S, typename E>
    State transition (S&& state, E)
    {
        return state;
    }

    // As we make use of exit/entry functions, we must handle all cases.
    // The default entry/exit action is an immediate return.
    template <typename S> void exit (variantFsm::TagType<S>) {}
    template <typename S> void entry (S&) {}

    // Functions required for the wakeupCall interface. waitFor() and
    // id() are not used for the wakeup calls in the state machine.
    void ring (uint64_t currentTime);
    void waitFor () {}
    size_t id () { return 0; }

private:
    RL01_02* context_ {nullptr};
    SimulatorClock::duration spinUpTime_;
};

#endif // _RL01_02_H_