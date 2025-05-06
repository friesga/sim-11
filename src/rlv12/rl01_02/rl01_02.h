#ifndef _RL01_02_H_
#define _RL01_02_H_

#include "unit/unit.h"
#include "unit/geometry/geometry.h"
#include "types.h"
#include "statuscodes.h"
#include "chrono/alarmclock/alarmclock.h"
#include "variantfsm/fsm.h"
#include "rlv12/rlv12command/rlv12command.h"
#include "panel.h"
#include "dummycontrols/dummycontrols.h"
#include "asynctimer/asynctimer.h"

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
using std::chrono::duration;

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
    RL01_02 (AbstractBusDevice* owningDevice);
    ~RL01_02 ();

    bool available ();
    u16 driveStatus ();
    bool volumeCheck () const;
    pair<bool, size_t> readData (RLV12Command& rlv12Command, u16* buffer,
        HeadPositionProcedure procedure, u16 diskAddressRegister);
    pair<bool, size_t> writeData (RLV12Command& rlv12Command, u16* buffer,
        size_t numWords, HeadPositionProcedure procedure, u16 diskAddressRegister);
    u16 readHeader (RLV12Command& rlv12Command, HeadPositionProcedure procedure,
        u16 diskAddressRegister);
    void seek (u16 diskAddressRegister);

    void setDriveReady ();
    void clearDriveReady ();
    bool driveReady ();
    void waitForDriveReady ();
    void setSeekIncomplete ();
    void waitForSeekComplete ();
    void resetDriveError ();

    StatusCode init (shared_ptr<RLUnitConfig> rlUnitConfig,
        Window* window);
    StatusCode init (shared_ptr<RLUnitConfig> rlUnitConfig);

private:
    // Definition of the drive states
    struct Initial {};      // State machine initial state
    struct Unloaded {};     // No cartridge loaded
    struct SpinningUp {};   // The drive is spinning up
    struct LockedOn {};     // The drive is locked on a cylinder
    struct Seeking {};      // The drive is seeking
    struct SpinningDown {}; // The drive is spinning down

    using State = std::variant <Initial, Unloaded, SpinningUp, LockedOn,
        Seeking, SpinningDown>;

    // Definition of the drive events
    struct SpinUp {};       // LOAD button pressed down
    struct SpinDown {};     // LOAD button released
    struct SpunUp {};       // Spin up is complete
    struct SpunDown {};     // Spin down is complete
    struct SeekCommand { SimulatorClock::duration seekTime; };
    struct TimeElapsed {};

    using Event = std::variant <SpinUp, SpinDown, SpunUp, SpunDown,
        SeekCommand, TimeElapsed>;

    // All RLV12Commands need access to the file pointer and unit status
    friend class CmdProcessor;
    
    // Use the PIMPL idiom to be able to define the StateMachine outside
    // of the RlDrive class
    class StateMachine;
    unique_ptr<StateMachine> stateMachine_;

    Geometry geometry_;
    u32 capacity_;                      // Drive capacity in words
    int32_t currentDiskAddress_;

    // The driveState_ keeps track of bits 0-5 of the MPR for a Get Status
    // command. The driveState_ is only set in the drive thread and is only
    // read by the command processor thread.
    // 
    // The driveStatus_ is the complete MPR for the Get Status command.
    u16 driveState_ {0};
    u16 driveStatus_ {0};

    bool running_ {false};
    AlarmClock alarmClock_;
    SimulatorClock::duration spinUpTime_ {0};

    // Calculated seek time
    SimulatorClock::duration seekTime_;

    // Safe guard against simultaneous access of the eventQueue_
    mutex driveMutex_;

    // Definitions to keep track of the drive ready state.
    size_t seeksInProgress_ {0};
    mutex driveReadyMutex_;
    condition_variable driveReadyCondition_;

    // Condition variable to wake up the seek timer
    condition_variable startCommand_;

    // Thread simulating drive timing
    thread driveThread_;

    // The queue by which commands and timeElapsed events are sent to
    // the drive thread.
    queue<Event> eventQueue_;

    // The drive thread for this unit
    void driveThread ();

    void sendTrigger (Event event);

    // Buttons and indicators. The initial value points to a dummy to
    // avoid null pointer references in the unit tests.
    DummyIndicator dummyIndicator_ {};
    DummyButton dummyButton_ {};
    DummyIndicatorButton dummyIndicatorButton_ {};

    IndicatorButton* loadButton_ {&dummyIndicatorButton_};
    Indicator* readyIndicator_ {&dummyIndicator_};
    Indicator* faultIndicator_ {&dummyIndicator_};
    Button* writeProtectButton_ {&dummyButton_};

    // Button and indicators positions and dimensions
    Frame<float> loadButtonFrame         {0.703, 0.538, 0.030, 0.060};
    Frame<float> readyIndicatorFrame     {0.743, 0.538, 0.030, 0.060};
    Frame<float> faultIndicatorFrame     {0.783, 0.538, 0.030, 0.060};
    Frame<float> writeProtectButtonFrame {0.823, 0.538, 0.030, 0.060};

    StatusCode configure (shared_ptr<RLUnitConfig> rlUnitConfig);
    void createBezel (Window* window, shared_ptr<RLUnitConfig> rlUnitConfig);
    int32_t filePosition (int32_t diskAddress) const;
    void updateHeadPosition (HeadPositionProcedure procedure,
        s32 wordCount, u16 diskAddressRegister);

    void loadButtonClicked (Button::State state);
    void writeProtectButtonClicked (Button::State state);
    Bitmask<AttachFlags> getAttachFlagsFromConfig (
        shared_ptr<RLUnitConfig> rlUnitConfig);
    void setDriveParameters (RLUnitConfig::RLUnitType unitType,
        t_offset fileSize);
};

// Definition of the state machine for the drive. The class has to be defined
// in the same compilation unit to prevent incomplete type compilation errors.
class RL01_02::StateMachine :
    public variantFsm::Fsm<StateMachine, Event, State>,
    public WakeUpCall
{
public:
    StateMachine (RL01_02* context, 
        duration<int, std::ratio<1, 1>> spinUpTime);

    State transition (Initial&&, SpunUp);           // -> LockedOn
    State transition (Initial&&, SpunDown);         // -> Unloaded
    void entry (Unloaded);
    State transition (Unloaded&&, SpinUp);          // -> SpinningUp
    void entry (SpinningUp);
    State transition (SpinningUp&&, TimeElapsed);   // -> LockedOn
    State transition (SpinningUp&&, SpinDown);      // -> SpinningDown
    void entry (LockedOn);
    State transition (LockedOn&&, SeekCommand);     // -> Seeking
    void exit (variantFsm::TagType<LockedOn>);
    void entry (Seeking);
    State transition (Seeking&&, TimeElapsed);      // -> LockedOn
    State transition (LockedOn&&, SpinDown);        // -> SpinningDown
    State transition (Seeking&&, SpinDown);         // -> SpinningDown
    void entry (SpinningDown);
    State transition (SpinningDown&&, TimeElapsed); // -> Unloaded
    State transition (SpinningDown&&, SpinUp);      // -> SpinningUp

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

    AsyncTimer spinUpDownTimer_;
    int timerId_ {0};

    void spinUpDownTimerExpired ();

    // Functions required for the wakeupCall interface
    void ring (uint64_t currentTime);
    void waitFor () {}
    size_t id () { return 0; }

private:
    RL01_02* context_ {nullptr};
    duration<int, std::ratio<1, 1>> spinUpTime_;
};

#endif // _RL01_02_H_