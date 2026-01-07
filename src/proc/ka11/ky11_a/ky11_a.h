#ifndef _KY11_A_H_
#define _KY11_A_H_

#include "bus/include/bus.h"
#include "proc/include/cpucontrol.h"
#include "proc/ka11/ka11machinestate/ka11machinestate.h"
#include "configdata/ka11/ky11_aconfig/ky11_aconfig.h"
#include "switchregister/switchregister.h"
#include "addressregister/addressregister.h"
#include "dataregister/dataregister.h"
#include "panel.h"
#include "bitfield.h"
#include "types.h"
#include "variantfsm/fsm.h"

#include <utility>
#include <string>
#include <array>
#include <functional>
#include <variant>

using std::pair;
using std::string;
using std::array;
using std::bind;
using std::placeholders::_1;
using std::variant;

class KY11_A
{
public:
    KY11_A (Bus* bus, Interfaces::CpuController* cpuController,
        Window* window, const KY11_AConfig& ky11_aConfig);

    // Definition of the KY11-A states
    struct AddressLoaded {};
    struct ExamineSequence {};
    struct DepositSequence {};
    struct Running {};

    using State = variant <AddressLoaded, ExamineSequence,
        DepositSequence, Running, monostate>;

    // Definition of the KY11-A events
    struct EXAM_Pressed {};
    struct DEP_Pressed {};
    struct LOAD_ADDR_Pressed {};
    struct START_Pressed {};

    using Event = variant <EXAM_Pressed, DEP_Pressed,
        LOAD_ADDR_Pressed, START_Pressed>;

private:
    Bus* bus_;
    Interfaces::CpuController* cpuController_;
    unique_ptr<SwitchRegister> switchRegister_ {};
    unique_ptr<AddressRegister> addressRegister_ {};
    unique_ptr<DataRegister> dataRegister_ {};

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

    Indicator* runLight_;

    void createBezel (Window* window, const KY11_AConfig& ky11_aConfig,
        unique_ptr<PanelBuilder>& panelBuilder);
    void powerSwitchClicked (Button::State state);
    void loadAddressSwitchClicked (Button::State state);
    void examSwitchClicked (Button::State state);
    void depSwitchClicked (Button::State state);
    void enableHaltSwitchClicked (Button::State state);
    void startSwitchClicked (Button::State state);
};

// Definition of the state machine for the KY11-A. The class has to be defined
// in the same compilation unit to prevent incomplete type compilation errors.
//
class KY11_A::StateMachine : public variantFsm::Fsm<StateMachine, Event, State>
{
public:
    StateMachine (KY11_A* context);

    State transition (AddressLoaded&&, LOAD_ADDR_Pressed);      // -> AddressLoaded
    State transition (AddressLoaded&&, EXAM_Pressed);           // -> ExamineSequence
    State transition (AddressLoaded&&, DEP_Pressed);            // -> DepositSequence
    State transition (AddressLoaded&&, START_Pressed);          // -> Running
    State transition (ExamineSequence&&, LOAD_ADDR_Pressed);    // -> AddressLoaded
    State transition (ExamineSequence&&, EXAM_Pressed);         // -> ExamineSequence
    State transition (ExamineSequence&&, DEP_Pressed);          // -> ExamineSequence
    State transition (ExamineSequence&&, START_Pressed);        // -> Running
    State transition (DepositSequence&&, LOAD_ADDR_Pressed);    // -> AddressLoaded
    State transition (DepositSequence&&, EXAM_Pressed);         // -> ExamineSequence
    State transition (DepositSequence&&, DEP_Pressed);          // -> DepositSequence
    State transition (DepositSequence&&, START_Pressed);        // -> Running

    // Define the default transition for transitions not explicitly
    // defined above. The default transition implies the event is ignored.
    template <typename S, typename E>
    State transition (S&& state, E)
    {
        return monostate {};
    }
    
private:
    KY11_A* context_ {};
};

#endif // _KY11_A_H_