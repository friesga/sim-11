#ifndef _KY11_A_H_
#define _KY11_A_H_

#include "bus/include/bus.h"
#include "proc/include/cpucontroller.h"
#include "proc/ka11/ka11machinestate/ka11machinestate.h"
#include "configdata/ka11/ky11_aconfig/ky11_aconfig.h"
#include "switchregister/switchregister.h"
#include "addressregister/addressregister.h"
#include "dataregister/dataregister.h"
#include "panel.h"
#include "bitfield.h"
#include "types.h"
#include "variantfsm/fsm.h"
#include "proc/include/ky11console.h"
#include "abstractbusdevice/abstractbusdevice.h"
#include "devicecommon/registerhandler/registerhandler.h"

#include <utility>
#include <string>
#include <array>
#include <functional>
#include <variant>
#include <vector>

using std::pair;
using std::string;
using std::array;
using std::bind;
using std::placeholders::_1;
using std::variant;
using std::vector;

class KY11_A : public KY11Console, public AbstractBusDevice
{
public:
    friend class KY11_ATest;
    friend class KY11_ATest_initialState_Test;
    friend class KY11_ATest_addressCanBeLoaded_Test;
    friend class KY11_ATest_addressCanBeExamined_Test;
    friend class KY11_ATest_addressSequenceCanBeExamined_Test;
    friend class KY11_ATest_loadAddressResetsExamineSequence_Test;
    friend class KY11_ATest_dataCanBeDeposited_Test;
    friend class KY11_ATest_dataSequenceCanBeDeposited_Test;
    friend class KY11_ATest_loadAddressResetsDepositSequence_Test;
    friend class KY11_ATest_examResetsDepositSequence_Test;
    friend class KY11_ATest_depResetsExamineSequence_Test;
    friend class KY11_ATest_startWhileEnabled_Test;
    friend class KY11_ATest_startResetsExamineSequence_Test;
    friend class KY11_ATest_startResetsDepositSequence_Test;
    friend class KY11_ATest_haltHaltsProgramOperation_Test;
    friend class KY11_ATest_resetLoadsTempRegister_Test;

    KY11_A (Bus* bus, Interfaces::CpuController* cpuController,
        Window* window, const KY11_AConfig& ky11_aConfig);

    // Functions required by the KY11Console interface
    HaltEnablePosition haltEnablePosition () const override;
    void display (BusAddress address, CondData<u16> data) override;

    // Functions required by the AbstractBusDevice class
    CondData<u16> read (BusAddress address);
    StatusCode writeWord (BusAddress address, u16 value);
    bool responsible (BusAddress address);
    void reset ();
    
    // Definition of the KY11-A states
    struct Off {};
    struct AddressLoaded {};
    struct ExamineSequence {};
    struct DepositSequence {};
    struct ProgramOperation {};

    using State = variant <Off, AddressLoaded, ExamineSequence,
        DepositSequence, ProgramOperation, monostate>;

    // Definition of the KY11-A events
    struct BPOK_High {};
    struct EXAM_Pressed {};
    struct DEP_Pressed {};
    struct LOAD_ADDR_Pressed {};
    struct START_Pressed {};
    struct CONT_Pressed {};
    struct HALT_Pressed {};

    using Event = variant <BPOK_High, EXAM_Pressed, DEP_Pressed,
        LOAD_ADDR_Pressed, START_Pressed, CONT_Pressed, HALT_Pressed>;

private:
    static constexpr u16 switchRegisterAddress {0177570};

    Bus* bus_;
    Interfaces::CpuController* cpuController_;
    unique_ptr<SwitchRegister> switchRegister_ {};
    unique_ptr<AddressRegister> addressRegister_ {};
    unique_ptr<DataRegister> dataRegister_ {};
    u16 tempRegister_ {0};

    // Use the PIMPL idiom to be able to define the StateMachine outside
    // of the KY11_A class
    class StateMachine;
    unique_ptr<StateMachine> stateMachine_;

    Button* powerSwitch_;
    Button* loadAddressSwitch_;
    Button* examineSwitch_;
    Button* depositSwitch_;
    Button* enableHaltSwitch_;
    Button* startSwitch_;
    Button* continueSwitch_;
    Button* singleInstructionCycleSwitch_;

    Indicator* runLight_;

    // The initial position of the ENABLE/HALT switch must correspond with
    // the initial state of enableHaltSwitch_ as defined in createBezel().
    Button::State enableHaltSwitchState_ {Button::TwoPositionsState::Down};
    HaltEnablePosition currentHaltEnablePosition_ {HaltEnablePosition::Halt};

    void createBezel (Window* window, const KY11_AConfig& ky11_aConfig,
        unique_ptr<PanelBuilder>& panelBuilder);

    void BPOKReceiver (bool signalValue);
    void SRUNReceiver (bool signalValue);

    // Functions to operate the KY11-A console switches
    void powerSwitchClicked (Button::State state);
    void loadAddressSwitchClicked (Button::State state);
    void examSwitchClicked (Button::State state);
    void depSwitchClicked (Button::State state);
    void enableHaltSwitchClicked (Button::State state);
    void startSwitchClicked (Button::State state);
    void continueSwitchClicked (Button::State state);
    void singleInstructionCycleSwitchClicked (Button::State state);
    void lockPanel ();
    void unlockPanel ();
};

// Definition of the state machine for the KY11-A. The class has to be defined
// in the same compilation unit to prevent incomplete type compilation errors.
//
class KY11_A::StateMachine : public variantFsm::Fsm<StateMachine, Event, State>
{
public:
    StateMachine (KY11_A* context);

    State transition (Off&&, BPOK_High);                        // -> AddressLoaded/ProgramOperation
    State transition (AddressLoaded&&, LOAD_ADDR_Pressed);      // -> AddressLoaded
    State transition (AddressLoaded&&, EXAM_Pressed);           // -> ExamineSequence
    State transition (AddressLoaded&&, DEP_Pressed);            // -> DepositSequence
    State transition (AddressLoaded&&, START_Pressed);          // -> ProgramOperation
    State transition (AddressLoaded&&, CONT_Pressed);           // -> ProgramOperation
    State transition (ExamineSequence&&, LOAD_ADDR_Pressed);    // -> AddressLoaded
    State transition (ExamineSequence&&, EXAM_Pressed);         // -> ExamineSequence
    State transition (ExamineSequence&&, DEP_Pressed);          // -> ExamineSequence
    State transition (ExamineSequence&&, START_Pressed);        // -> ProgramOperation
    State transition (ExamineSequence&&, CONT_Pressed);         // -> ProgramOperation
    State transition (DepositSequence&&, LOAD_ADDR_Pressed);    // -> AddressLoaded
    State transition (DepositSequence&&, EXAM_Pressed);         // -> ExamineSequence
    State transition (DepositSequence&&, DEP_Pressed);          // -> DepositSequence
    State transition (DepositSequence&&, START_Pressed);        // -> ProgramOperation
    State transition (DepositSequence&&, CONT_Pressed);         // -> ProgramOperation
    State transition (ProgramOperation&&, HALT_Pressed);        // -> AddressLoaded

    // Define the default transition for transitions not explicitly
    // defined above. The default transition implies the event is ignored.
    template <typename S, typename E>
    State transition (S&& state, E)
    {
        return monostate {};
    }

private:
    KY11_A* context_ {};

    State startPressed (State currentState);
    State contPressed (State currentState);
};

#endif // _KY11_A_H_