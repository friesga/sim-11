#ifndef _CONTROLLOGIC_H_
#define _CONTROLLOGIC_H_

#include "kd/include/pdp11processor.h"
#include "kd/include/kd11odt.h"
#include "kd/kd11_na/cpu/kd11_na_cpu.h"
#include "kd/kd11_na/operatorconsoleaccess/operatorconsoleaccess.h"
#include "kd/include/kd11odt.h"
#include "configdata/kd11_naconfig/kd11_naconfig.h"
#include "variantfsm/fsm.h"
#include "threadsafecontainers/threadsafequeue.h"

#include <memory>

using std::unique_ptr;
using std::shared_ptr;
using std::unique_lock;
using std::defer_lock;
using std::monostate;
using std::bind;
using std::placeholders::_1;
using std::placeholders::_2;
using std::placeholders::_3;

// The ControlLogic contains the Power-Up/Power-Down/Restart/Halt logic. The
// logic is a state machine and always is in one of the following states:
// Power Off - The system is switched off,
// Halted - The processor is halted and ODT is executed,
// Running - The processor is running,
// PowerFail - The AC power is failing and DC power will drop in a few
//             milliseonds.
//
// The control logic transitions through these states on the basis of the
// bus signals.
//
class ControlLogic
{
public:
    // The constructor has the following parameters:
    // bus - Pointer to the bus to use,
    // cpu - Pointer to the cpu in the system, eg a KD11_NA or KDF11-A,
    // powerUpMode - The mode to switch to when powered up,
    // startAddress - The address to set the PC to when starting up,
    // odtCreator - A std::function for a function to create an ODT object
    //
    ControlLogic (Qbus* bus, CpuData* cpu, 
        KD11_NAConfig::PowerUpMode powerUpMode, u16 startAddress,
        KD11ODT::Creator odtCreator);
    ~ControlLogic ();
    void run ();

    // Declare the signal receivers
    void BHALTReceiver (bool signalValue);
    void BPOKReceiver (bool signalValue);
    void ExitReceiver (bool signalValue);
    void ResetReceiver (bool signalValue);

private:
    // Definition of the events to be processed by the state machine
    struct BPOK_high {};
    struct Halt {};
    struct Start {};
    struct Reset {};
    struct BPOK_low {};
    struct BDCOK_low {};
    struct Exit {};

    using Event = std::variant<BPOK_high,
        Halt,
        Start,
        Reset,
        BPOK_low,
        BDCOK_low,
        Exit>;

    // Definition of the states
    struct PowerOff {};
    struct Running {};
    struct Halted {};
    struct PowerFail {};
    struct ExitPoint {};

    using State = std::variant<PowerOff,
        Running,
        Halted,
        PowerFail,
        ExitPoint,
        monostate>;

    // Use the PIMPL idiom to be able to define the StateMachine outside
    // of the Switch class
    class StateMachine;
    unique_ptr<StateMachine> stateMachine_;

    Qbus* bus_;
    CpuData* cpu_;
    unique_ptr<KD11ODT>	odt_ {};
    KD11_NAConfig::PowerUpMode powerUpMode_;
    u16 startAddress_;
    KD11ODT::Creator odtCreator_;
    bool kd11Running_;

    // Definition of a queue for the processing of bus signal events
    ThreadSafeQueue<Event> signalEventQueue_;

    InterruptRequest const powerFail {RequestType::Trap, TrapPriority::PowerFail, 0, 024};

    State powerUpRoutine ();
    void subscribeToSignals ();
    void runODT ();
    bool signalAvailable ();
    template <typename T> bool signalIsOfType ();
};

// This function returns true if the first element in the signal queue is
// a variant holding an Event of the specified type.
template <typename T>
bool ControlLogic::signalIsOfType ()
{
    return holds_alternative<T> (signalEventQueue_.first ());
}

class ControlLogic::StateMachine : public variantFsm::Fsm<StateMachine, Event, State>
{
public:
    StateMachine (ControlLogic* context);

    // Definition of the control logic state machine
    State transition (PowerOff&&, BPOK_high);		// -> Halted/Running
    void entry (Running);
    State transition (Running&&, Reset);			// -> Halted/Running
    State transition (Running&&, Halt);				// -> Halted
    State transition (Running&&, BPOK_low);			// -> PowerFail
    void entry (Halted);
    State transition (Halted&&, Start);				// -> Running
    State transition (Halted&&, Reset);				// -> Halted/Running
    State transition (Halted&&, BPOK_low);			// -> PowerOff
    void entry (PowerFail);
    State transition (PowerFail&&, BDCOK_low);		// -> PowerOff
    State transition (PowerFail&&, Halt);			// -> PowerOff

    template <typename S>
    State transition (S&& state, Exit)
    {
        return ExitPoint {};
    }
    void entry (ExitPoint);

    // Define the default transition for transitions not explicitly
    // defined above. The default transition implies the event is ignored.
    template <typename S, typename E>
    State transition (S&& state, E)
    {
        return monostate {};
    }

    // As we make use of exit/entry functions, we must handle all cases.
    // The default entry/exit action is an immediate return.
    template <typename S> void exit (variantFsm::TagType<S>) {}
    template <typename S> void entry (S&) {}

private:
    ControlLogic *context_;
};

inline ControlLogic::StateMachine::StateMachine (ControlLogic* context)
    :
    context_ {context}
{}

#endif // !_CONTROLLOGIC_H_
