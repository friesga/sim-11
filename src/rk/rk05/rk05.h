#ifndef _RK05_H_
#define _RK05_H_

#include "diskdrive/diskdrive.h"
#include "bus/include/bus.h"
#include "configdata/rk/rk05/rk05config/rk05config.h"
#include "rk/include/rktypes.h"
#include "panel.h"
#include "asynctimer/asynctimer.h"
#include "variantfsm/fsm.h"
#include "chrono/simulatorclock/simulatorclock.h"
#include "dummycontrols/dummycontrols.h"

#include <thread>
#include <queue>
#include <memory>

using std::shared_ptr;
using std::unique_ptr;
using std::thread;
using std::queue;

class RK05
{
public:
    RK05 (Bus* bus, AbstractBusDevice* controller, Window* window,
        shared_ptr<RK05Config> rk05Config);
    ~RK05 ();
    void processCommand (RKTypes::Function action);

private:
    Bus* bus_ {nullptr};
    AbstractBusDevice* controller_ {nullptr};

    // Definition of the RK05 drive format:
    // - 12 sectors/track
    // - 2 disk surfaces/disk
    // - 203 cylinders/disk drive
    // - 256 words/sector
    Geometry rk05Geometry {12, 2, 203, 256};

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

    // Button and indicators positions and dimensions
    Frame<float> pwrIndicatorFrame {0.674, 0.605, 0.024, 0.048};
    Frame<float> readyIndicatorFrame {0.703, 0.605, 0.024, 0.048};
    Frame<float> oncylIndicatorFrame {0.732, 0.605, 0.024, 0.048};
    Frame<float> faultIndicatorFrame {0.761, 0.605, 0.024, 0.048};
    Frame<float> wtprotIndicatorFrame {0.674, 0.690, 0.024, 0.048};
    Frame<float> loadIndicatorFrame {0.703, 0.690, 0.024, 0.048};
    Frame<float> wtIndicatorFrame {0.732, 0.690, 0.024, 0.048};
    Frame<float> rdIndicatorFrame {0.761, 0.690, 0.024, 0.048};
    Frame<float> runLoadSwitchFrame {0.585, 0.605, 0.029, 0.122};
    Frame<float> wtprotSwitchFrame {0.613, 0.605, 0.029, 0.122};

    // Definition of the drive states
    struct Initial {};      // State machine initial state
    struct Unloaded {};     // No cartridge loaded
    struct SpinningUp {};   // The drive is spinning up
    struct LockedOn {};     // The drive is locked on a cylinder
    struct Seeking {};      // The drive is seeking
    struct SpinningDown {}; // The drive is spinning down

    using State = std::variant <Initial, Unloaded, SpinningUp, LockedOn,
        Seeking, SpinningDown>;

    // Definition of the drive events. This includes the RKCommand defined
    // in rktypes.h.
    struct SpinUp {};       // LOAD button pressed down
    struct SpinDown {};     // LOAD button released
    struct SpunUp {};       // Spin up is complete
    struct SpunDown {};     // Spin down is complete
    struct SeekCommand { SimulatorClock::duration seekTime; };
    struct TimeElapsed {};

    using Event = std::variant <SpinUp, SpinDown, SpunUp, SpunDown,
        SeekCommand, TimeElapsed, RKTypes::Function>;

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

    void createBezel (Window* window, shared_ptr<RK05Config> rk05Config);
    void runLoadSwitchClicked (Button::State state);
    void wtprotSwitchClicked (Button::State state);
    void driveThread ();
    void sendTrigger (Event event);
    Bitmask<AttachFlags> getAttachMode (
        shared_ptr<RK05Config> rk05Config);
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
    State transition (LockedOn&&, RKTypes::Function);    // -> LockedOn
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

    void handleFunction (RKTypes::Function action);
};

#endif // _RK05_H_