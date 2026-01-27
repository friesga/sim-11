#include "ky11_a.h"
#include "configdata/ka11/ky11_aconfig/ky11_aconfig.h"
#include "panel.h"
#include "imagedata/openrasterfile/openrasterfile.h"

#include <memory>
#include <functional>

using std::bind;
using std::placeholders::_1;
using std::placeholders::_2;

using std::make_unique;

KY11_A::KY11_A (Bus* bus, Interfaces::CpuController* cpuController,
    Window* window, const KY11_AConfig& ky11_aConfig)
    :
    bus_ {bus},
    cpuController_ {cpuController}
{
    stateMachine_ = make_unique<StateMachine> (this);

    unique_ptr<ImageContainer> imageContainer =
        make_unique<OpenRasterFile> ("resources/pdp-11_20 front.ora");

    unique_ptr<PanelBuilder> panelBuilder =
        window->createDataPanelBuilder (*imageContainer,
            *ky11_aConfig.cabinetPosition, ky11_aConfig.unitHeight);

    createBezel (window, ky11_aConfig, panelBuilder);

    switchRegister_ = make_unique<SwitchRegister> (panelBuilder);
    addressRegister_ = make_unique<AddressRegister> (panelBuilder);
    dataRegister_ = make_unique<DataRegister> (panelBuilder);

    window->addPanel (panelBuilder->getPanel ());

    bus_->BPOK ().subscribe (bind (&KY11_A::BPOKReceiver, this, _1));
    bus_->SRUN ().subscribe (bind (&KY11_A::SRUNReceiver, this, _1));
}

void KY11_A::createBezel (Window* window, const KY11_AConfig& ky11_aConfig,
    unique_ptr<PanelBuilder>& panelBuilder)
{
    panelBuilder->createFront ("panel");

    runLight_ = panelBuilder->createIndicator ("run_off", "run_on",
        Indicator::State::Off);

    powerSwitch_ = panelBuilder->createMultiPositionSwitch (
        {"keyswitch off",
        "keyswitch power",
        "keyswitch lock",
        },
        Button::ThreePositionsState::Left,
        bind (&KY11_A::powerSwitchClicked, this, _1));

    loadAddressSwitch_ = panelBuilder->createMultiPositionSwitch (
        {"load_addr_up", "load_addr_down"},
        Button::MomentaryDownTwoPositionsState::Up,
        bind (&KY11_A::loadAddressSwitchClicked, this, _1));

    examineSwitch_ = panelBuilder->createMultiPositionSwitch (
        {"exam_up", "exam_down"},
        Button::MomentaryDownTwoPositionsState::Up,
        bind (&KY11_A::examSwitchClicked, this, _1));

    depositSwitch_ = panelBuilder->createMultiPositionSwitch (
        {"deposit_down", "deposit_up"},
        Button::MomentaryUpTwoPositionsState::Down,
        bind (&KY11_A::depSwitchClicked, this, _1));

    enableHaltSwitch_ = panelBuilder->createMultiPositionSwitch (
        {"enable_halt_down", "enable_halt_up"},
        Button::TwoPositionsState::Up,
        bind (&KY11_A::enableHaltSwitchClicked, this, _1));

    startSwitch_ = panelBuilder->createMultiPositionSwitch (
        {"start_up", "start_down"},
        Button::MomentaryDownTwoPositionsState::Up,
        bind (&KY11_A::startSwitchClicked, this, _1));

    continueSwitch_ = panelBuilder->createMultiPositionSwitch (
        {"cont_up", "cont_down"},
        Button::MomentaryDownTwoPositionsState::Up,
        bind (&KY11_A::continueSwitchClicked, this, _1));

    singleInstructionCycleSwitch_ = panelBuilder->createMultiPositionSwitch (
        {"sinst_scycle_up", "sinst_scycle_down"},
        Button::MomentaryDownTwoPositionsState::Up,
        bind (&KY11_A::singleInstructionCycleSwitchClicked, this, _1));
}

void KY11_A::powerSwitchClicked (Button::State state)
{
    switch (get<Button::ThreePositionsState> (state))
    {
        case Button::ThreePositionsState::Left:
            // Power off
            bus_->BPOK ().set (false);
            break;

        case Button::ThreePositionsState::Center:
            // Power on
            bus_->BPOK ().set (true);
            break;

        case Button::ThreePositionsState::Right:
            // Panel locked
            break;
    }
}

// The LOAD ADDR switch transfers the switch register contents to the Bus
// Address Register (BAR) [...]. This bus address, displayed in the ADDRESS
// REGISTER, provides an address for the console functions of EXAM, DEP and
// START. (DEC-11-HR1B-D Table 3-2)
//
void KY11_A::loadAddressSwitchClicked (Button::State state)
{
    if (get<Button::MomentaryDownTwoPositionsState> (state) ==
        Button::MomentaryDownTwoPositionsState::Down)
    {
        stateMachine_->dispatch (LOAD_ADDR_Pressed {});
    }
}

// The EXAM switch transfers the contents of the bus address (which is
// specified by the Bus Address Register) for DATA display. After use, the
// data appear on the DATA display and the address of the data is in the
// ADDRESS REGISTER.
// 
// A LOAD ADDR operation pre-establishes the initial address; sequential
// addresses occur automatically.
// 
// If the EXAM switch is depressed twice in
// succession, the contents of the next sequential bus address are displayed
// in DATA. This action is repeated each time EXAM is depressed provided no
// other switch is used between these steps.
//
// Whenever LOAD ADDR or DEP switch is used, it destroys the incrementing
// sequence.The next time EXAM is used, it displays the current Bus Address
// Register address rather than the next sequential address.
//
// Source: DEC-11-HR1B-D, Table 3-2.
//
void KY11_A::examSwitchClicked (Button::State state)
{
    if (get<Button::MomentaryDownTwoPositionsState> (state) ==
        Button::MomentaryDownTwoPositionsState::Down)
    {
        stateMachine_->dispatch (EXAM_Pressed {});
    }
}

// The DEP switch transfers the contents of the console SWITCH REGISTER to
// the bus address (specified by Bus Adddress Register). After use, the data
// appear on the DATA display and the address is in the ADDRESS REGISTER.
// A LOAD ADDR operation pre-establishes the initial address; sequential
// addresses occur automatically.
// 
// If the DEP switch is raised twice in succession, the contents of the SWITCH
// REGISTER is deposited in the next sequential bus address location. The
// action is repeated each time DEP is raised provided no other switch is used
// between these steps. Whenever the LOAD ADDR or EXAM switch is depressed, the
// incrementing sequence is destroyed. The next time DEP is used, it deposits
// the current address rather than the next sequential address.
// 
// Source: DEC-11-HR1B-D, Table 3-2.
//
void KY11_A::depSwitchClicked (Button::State state)
{
    if (get<Button::MomentaryUpTwoPositionsState> (state) ==
        Button::MomentaryUpTwoPositionsState::Down)
    {
        stateMachine_->dispatch (DEP_Pressed {});
    }
}

// The ENABLE/HALT switch allows either the program or the console to control
// processor operation.
// 
// ENABLE position - permits the system to run in a normal manner. No console
// control requests (type dependent upon SINST/S - CYCLE switch) are made.
// Without console control, all switches except ENABLE/HALT and the SWITCH
// REGISTER are disabled.
//
// HALT position - halts the processor and passes control to the console; with
// the SINST/S-CYCLE switch in the S-CYCLE mode, console switches (except
// SWITCH REGISTER, CONT, and ENABLE/HALT) are disabled unless the machine is
// stepped to the end of an instruction. In the SINST mode, the processor stops
// at the end of an instruction and all console switches are enabled.
// 
// The HALT mode is used with the CONT switch to step the machine through
// programs and facilitate intermediate observations.
// 
// When the START switch is activated in the HALT mode, a system clear is
// effected.
//
void KY11_A::enableHaltSwitchClicked (Button::State state)
{
    if (get<Button::TwoPositionsState> (state) ==
        Button::TwoPositionsState::Down)
    {
        bus_->BHALT ().set (true);
        stateMachine_->dispatch (HALT_Pressed {});
        currentHaltEnablePosition_ = KY11Console::HaltEnablePosition::Halt;
    }
    else
    {
        bus_->BHALT ().set (false);
        currentHaltEnablePosition_ = KY11Console::HaltEnablePosition::Enable;
    }
}

// When ENABLE/HALT is set to ENABLE, depressing START provides a system clear
// operation and then begins processor operation. A LOAD ADDR operation
// preestablishes the starting address.
//
// When ENABLE/HALT is set to HALT, depressing START provides a system clear
// (initialize) only. The processor does not start; the Bus Address Register
// is loaded from a temporary processor register (TEMP) which is usually
// pre-loaded by LOAD ADDR.
// 
// This provides the only method of reading TEMP when it does not contain the
// LOAD ADDR value.
//
void KY11_A::startSwitchClicked (Button::State state)
{
    if (get<Button::MomentaryDownTwoPositionsState> (state) ==
            Button::MomentaryDownTwoPositionsState::Down)
        stateMachine_->dispatch (START_Pressed {});
}


// Pressint the CONT switch causes the processor to continue operation from
// the previous point at which it had stopped.
//
// If the ENABLE/HALT switch is in the ENABLE mode, CONT returns bus control
// from the console to the processor and continues program operation.
//
// If the ENABLE/HALT switch is set to HALT, depressing the CONT key causes
// the processor to perform either a single instruction or a single bus cycle
// (dependent on position of the S-INST/S-CYCLE switch) and then stop. Bus
// control has been returned to the console and the CONT switch must be used
// to continue.
//
void KY11_A::continueSwitchClicked (Button::State state)
{
    if (get<Button::MomentaryDownTwoPositionsState> (state) ==
            Button::MomentaryDownTwoPositionsState::Down)
        stateMachine_->dispatch (CONT_Pressed {});
}

// The SINST/S-CYCLE switch allows the processor to step through program
// operation either one instruction or one bus cycle at a time. The user may
// note processor operation and contents of registers during major states or
// bus cycles of individual instructions.
//
// S-INST position - the processor halts after an instruction. This process is
// repeated each time the CONT key is depressed. Console switches (LOAD ADDR,
// EXAM, DEP) can be used when the processor halts.
//
// S-CYCLE position - The processor halts after a bus cycle. The process is
// repeated each time CONT is depressed. Console switches are inoperative
// unless the machine is stepped to the S-INST halt position. (Changing mode
// to S-INST and using CONT switch effects this.)
//
// ToDo: SINST/S-CYCLE switch to be implemented
//
void KY11_A::singleInstructionCycleSwitchClicked (Button::State state)
{
}

KY11Console::HaltEnablePosition KY11_A::haltEnablePosition () const
{
    return currentHaltEnablePosition_;
}

void KY11_A::BPOKReceiver (bool signalValue)
{
    if (signalValue)
        stateMachine_->dispatch (BPOK_High {});
    // else
    //    stateMachine_.push (BPOK_Low {});
}

void KY11_A::SRUNReceiver (bool signalValue)
{
    if (signalValue)
        runLight_->show (Indicator::State::On);
    else
        runLight_->show (Indicator::State::Off);
}

void KY11_A::setSwitchRegister (u16 value)
{
    *switchRegister_ = value;
}

KY11_A::KY11_AStatus KY11_A::getKY11_AStatus () const
{
    return KY11_AStatus {*addressRegister_};
}
