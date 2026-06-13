#include "ba11_l.h"
#include "imagedata/openrasterfile/openrasterfile.h"

//
// Support for the BA11-L Mounting Box with the PDP-11/24 front panel
//
#include <functional>
#include <map>
#include <memory>

using std::bind;
using std::placeholders::_1;
using std::map;
using std::make_unique;
using std::unique_ptr;

// Constructor
// Create a window showing the BA11-L start a thread handling the events
// and render the lamps and switches.
BA11_L::BA11_L (Bus* bus, Window* window, const BA11_LConfig& ba11lConfig)
    :
    bus_ {bus},
    frontWindow_ {window}
{
    createBezel (ba11lConfig.cabinetPosition.value ());
}

// Create the BA11-L panel at the specified position, width and height
// and then start a loop handling the events and rendering lamps and
// switches.
// 
// The operator switch panel contains the power switch and the
// HALT/CONT/BOOT switch.
// 
// The power switch is a four-position rotary keyswitch with the following
// positions:
// DC OFF   DC power is removed from the system; contents of MOS memory
//          are lost and system cooling fans are off.
// LOCAL    Power is applied to the system; enables all functions and
//          normal operation.
// LOC DSBL Power is present throughout the system.However, the
//          HALT/CONT/BOOT switch is disabled and the "break" key on the
//          terminal will not halt the CPU.
// STDBY    DC power to most of the computer is off but dc power is
//          applied to MOS memory to avoid data loss.
//
// The HALT/CONT/BOOT switch is a three-position latching/momentary switch
// with the following positions:
// HALT     Halts the processor after all pending interrupts and aborts
//          have been serviced.
// CONT     Continue.The processor is enabled for normal operation;
//          however, it will not begin executing instructions until
//          commanded to from the console.
// BOOT     Initializes the system.The processor will begin executing
//          instructions contained in the hardware bootstrap.
// 
// The HALT and CONT position are latching positions while the BOOT
// position is momentary.
// 
// At least for Windows, event handling has to be performed in the same
// thread as in which the window has been created.
// 
void BA11_L::createBezel (Cabinet::Position cabinetPosition)
{
    unique_ptr<ImageContainer> imageContainer =
        make_unique<OpenRasterFile> ("resources/PDP-11_24.ora");

    unique_ptr<PanelBuilder> panelBuilder =
        frontWindow_->createDataPanelBuilder (*imageContainer,
            cabinetPosition, BA11_LConfig::unitHeight);

    panelBuilder->createFront ("PDP-11/24 front");

    powerSwitch_ = panelBuilder->createMultiPositionSwitch (
        {"DC OFF",
         "LOCAL",
         "LOC DSBL",
         "STD BY"},
        Button::FourPositionsState::P0,
        bind (&BA11_L::powerSwitchClicked, this, _1));

    hcbSwitch_ = panelBuilder->createMultiPositionSwitch (
        {"HALT",
         "CONT",
         "BOOT"},
        Button::MomentaryThreePositionsState::Center,
        bind (&BA11_L::hcbSwitchClicked, this, _1));

    runLed_ = panelBuilder->createIndicator ("RUN off",
        "RUN on", Indicator::State::Off);
    dcOnLed_ = panelBuilder->createIndicator ("DC ON off",
        "DC ON on", Indicator::State::Off);
    batteryLed_ = panelBuilder->createIndicator ("BATT off",
        "BATT on", Indicator::State::Off);

    frontWindow_->addPanel (panelBuilder->getPanel ());

    // The state of the HALT signal has to correspond with the position
    // of the HALT/CONT/BOOT switch.
    bus_->BHALT ().set (false);

    // Subscribe to the signal indicating the state to be shown
    bus_->SRUN ().subscribe (bind (&BA11_L::SRUNReceiver, this, _1));
}

void BA11_L::powerSwitchClicked (Button::State state)
{
    switch (get<Button::FourPositionsState> (state))
    {
        case Button::FourPositionsState::P0:
            // DC OFF - DC power is removed from the system; contents of
            // MOS memory are lost and system cooling fans are off.
            dcOnLed_->show (Indicator::State::Off);
            batteryLed_->show (Indicator::State::Off);
            bus_->BatteryPower ().set (false);
            bus_->BPOK ().set (false);
            break;

        case Button::FourPositionsState::P1:
            // LOCAL - Power is applied to the system; enables all functions
            // and normal operation.
            dcOnLed_->show (Indicator::State::On);
            batteryLed_->show (Indicator::State::On);
            bus_->BPOK ().set (true);
            enableHCBSwitch ();
            break;

        case Button::FourPositionsState::P2:
            // LOC DSBL - Power is present throughout the system. However, the
            // HALT/CONT/BOOT  switch is disabled and the "break" key on the
            // terminal will not halt the CPU.
            // 
            // This state may be entered from both the LOCAL state and the 
            // STANDBY state.
            dcOnLed_->show (Indicator::State::On);
            bus_->BPOK ().set (true);
            disableHCBSwitch ();
            break;

        case Button::FourPositionsState::P3:
            // STANDBY - DC power to most of the computer is off but dc power is
            // applied to MOS memory to avoid data loss.
            dcOnLed_->show (Indicator::State::Off);
            bus_->BatteryPower ().set (true);
            bus_->BPOK ().set (false);
            break;
    }
}

void BA11_L::hcbSwitchClicked (Button::State state)
{
    switch (get<Button::MomentaryThreePositionsState> (state))
    {
        case Button::MomentaryThreePositionsState::Left:
            // HALT - Halt the processor
            bus_->BHALT ().set (true);
            break;

        case Button::MomentaryThreePositionsState::Center:
            // CONT - The processor is enabled for normal operation
            bus_->BHALT ().set (false);
            break;

        case Button::MomentaryThreePositionsState::Right:
            // BOOT - Initializes the system
            bus_->BOOT ().cycle ();
            break;
    }
}

// The RUN led reflects the state of the SRUN signal.
void BA11_L::SRUNReceiver (bool signalValue)
{
    runLed_->show (signalValue ? Indicator::State::On : Indicator::State::Off);
}

// When the power switch is set to the LOCAL DISABLE position, the
// HALT/CONT/BOOT switch and the console break key must be disabled.
// This is done by blocking the relevant signals.
void BA11_L::disableHCBSwitch ()
{
    blockSignals ();
}

// When the power switch is set to any other position then LOCAL DISABLE,
// the blocked signals have to be unblocked. The position of the
// HALT/CONT/BOOT switch may have be changed while the power key was in the
// LOCAL DISABLE position, so when the power switch is put in another
// position, the current position of the HALT/CONT/BOOT switch must be
// processed.
void BA11_L::enableHCBSwitch ()
{
    unblockSignals ();
    hcbSwitchClicked (hcbSwitch_->currentState ());
}

void BA11_L::blockSignals ()
{
    bus_->BHALT ().block ();
    bus_->RESET ().block ();
}

void BA11_L::unblockSignals ()
{
    bus_->BHALT ().unblock ();
    bus_->RESET ().unblock ();
}
