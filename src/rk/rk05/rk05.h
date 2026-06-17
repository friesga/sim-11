#ifndef _RK05_H_
#define _RK05_H_

#include "diskdrive/diskdrive.h"
#include "bus/include/bus.h"
#include "configdata/rk/rk05/rk05config/rk05config.h"
#include "rk/include/rktypes.h"
#include "rk/include/driveinterface.h"
#include "panel.h"
#include "asynctimer/asynctimer.h"
#include "variantfsm/fsm.h"
#include "chrono/simulatorclock/simulatorclock.h"
#include "dummycontrols/dummycontrols.h"
#include "chrono/simulatorclock/simulatorclock.h"

#include <thread>
#include <queue>
#include <memory>
#include <functional>
#include <chrono>

using std::unique_ptr;
using std::thread;
using std::queue;
using std::function;

using namespace std::chrono_literals;

class RK05
{
public:
    RK05 (Bus* bus, DriveInterface* controller, Window* window,
        const RK05Config& rk05Config);
    ~RK05 ();

    // Functions getting the state of the drive
    bool isReady ();
    bool isWriteProtected ();
    RKTypes::RKDS driveStatus ();

    // Functions implementing the RK11 function
    void seek (u16 cylinderAddress);
    RKTypes::FunctionResult write (DiskAddress diskAddress,
        u16 wordCount, u16* data);
    RKTypes::FunctionResult read (DiskAddress diskAddress,
        u16 wordCount, u16* data);
    RKTypes::FunctionResult readHeader (DiskAddress diskAddress,
        u16 wordCount, u16* data);
    void clearDriveReady ();
    void writeLock ();

private:
    class WriteCompletion;

    // Define revolution times for a RK05 disk
    SimulatorClock::duration indexPulseTime {40ms};
    SimulatorClock::duration sectorRevolutionTime {indexPulseTime / 12};

    Bus* bus_ {nullptr};
    DriveInterface* controller_ {nullptr};
    u16 driveId_;

    DiskDrive diskDrive_ {};
    unique_ptr<u16[]> buffer_ {};

    // Buttons and indicators. The initial value points to a dummy to
    // avoid null pointer references in the unit tests.
    DummyIndicator dummyIndicator_ {};
    DummyButton dummyButton_ {};

    Indicator* pwrIndicator_ {&dummyIndicator_};
    Indicator* rdyIndicator_ {&dummyIndicator_};
    Indicator* oncylIndicator_ {&dummyIndicator_};
    Indicator* faultIndicator_ {&dummyIndicator_};
    Indicator* wtprotIndicator_ {&dummyIndicator_};
    Indicator* loadIndicator_ {&dummyIndicator_};
    Indicator* wtIndicator_ {&dummyIndicator_};
    Indicator* rdIndicator_ {&dummyIndicator_};
    Button* runLoadSwitch_ {&dummyButton_};
    Button* wtprotSwitch_ {&dummyButton_};

    // Definition of the drive states
    //
    // To both the SeekCommand and the Seeking state a function pointer
    // can be passed. This function will be called after completion of
    // a seek. This allows different action on a async seek command initiated
    // by the controller and synced seeks for read and write commands.
    // The function pointer must be passed to the SeekCommand which will
    // then forward it to the Seeking state. On a transition from the
    // Seeking state to the LockedOn state the function will then be called.
    //
    struct Initial {};      // State machine initial state
    struct Unloaded {};     // No cartridge loaded
    struct SpinningUp {};   // The drive is spinning up
    struct LockedOn {};     // The drive is locked on a cylinder
    struct Seeking { function<void ()> seekCompleted {nullptr}; };  // The drive is seeking
    struct SpinningDown {}; // The drive is spinning down

    using State = std::variant <Initial, Unloaded, SpinningUp, LockedOn,
        Seeking, SpinningDown>;

    // Definition of the drive events. This includes the RKCommand defined
    // in rktypes.h.
    struct SpinUp {};       // LOAD button pressed down
    struct SpinDown {};     // LOAD button released
    struct SpunUp {};       // Spin up is complete
    struct SpunDown {};     // Spin down is complete
    struct SeekCommand { SimulatorClock::duration seekTime;
                            function<void ()> seekCompleted {nullptr}; };
    struct TimeElapsed {};

    using Event = std::variant <SpinUp, SpinDown, SpunUp, SpunDown,
        SeekCommand, TimeElapsed>;

    // Use the PIMPL idiom to be able to define the StateMachine outside
    // of the RK05 class
    class StateMachine;
    unique_ptr<StateMachine> stateMachine_;

    // Thread simulating drive timing
    bool running_ {false};
    thread driveThread_;

    // The queue by which commands and timeElapsed events are sent to
    // the drive thread.
    queue<Event> eventQueue_;

    // Safe guard against simultaneous access of the eventQueue_
    mutex driveMutex_;

    // Condition variable to wake up the drive thread when a command is issued
    condition_variable startCommand_;

    // RK05 state definitions. The drive keeps track of its own copies
    // of the drive status and error register the controller can use to
    // assign to the actual register.
    u16 currentCylinderAddress_ {0};
    RKTypes::RKDS driveStatus_ {0};
    RKTypes::RKER driveError_ {0};

    void createBezel (Window* window, const RK05Config& rk05Config);
    void runLoadSwitchClicked (Button::State state);
    void wtprotSwitchClicked (Button::State state);
    void driveThread ();
    void sendTrigger (Event event);
    Bitmask<AttachFlags> getAttachMode (const RK05Config& rk05Config);
    SimulatorClock::duration seekTime (u16 currentCylinderAddress,
        u16 newCylinderAddress);
    size_t readDataFromDrive (DiskAddress diskAddress, u16* buffer,
        u32 wordCount);
    size_t writeDataToDrive (DiskAddress diskAddress, u16* buffer,
        u16 numWords);
    void clearBufferToEndOfSector (u16* buffer, u16 numWords);
    u16 cylinderFromLBN (u32 lbn);
};


// Definition of the state machine for the drive. The class has to be defined
// in the same compilation unit to prevent incomplete type compilation errors.
class RK05::StateMachine :
    public variantFsm::Fsm<StateMachine, Event, State>,
    public WakeUpCall
{
public:
    StateMachine (RK05* context,
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
    State transition (LockedOn&&, SpinDown);        // -> SpinningDown
    void exit (variantFsm::TagType<LockedOn>);
    void entry (Seeking);
    State transition (Seeking&&, TimeElapsed);      // -> LockedOn
    State transition (Seeking&&, SpinDown);         // -> SpinningDown
    void entry (SpinningDown);
    State transition (SpinningDown&&, TimeElapsed); // -> Unloaded
    State transition (SpinningDown&&, SpinUp);      // -> SpinningUp

    // Define the default transition for transitions on the receipt
    // of a RKCommand not explicitly defined above. In these cases
    // an error is returned to the controller.  
    //
    template <typename S>
    State transition (S&& state, RKTypes::Function)
    {
        RKTypes::RKER rker {};
        rker.driveError = 1;

        // context_->controller_->processResult (RKTypes::Result {
        //    0, rker, 0, 0});
        return state;
    }

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
    RK05* context_ {nullptr};
    duration<int, std::ratio<1, 1>> spinUpTime_;
};

#endif // _RK05_H_