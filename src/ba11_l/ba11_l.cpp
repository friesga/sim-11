#include "ba11_l.h"

//
// Support for the BA11-L Mounting Box with the PDP-11/24 front panel
//
#include <functional>
#include <map>

using std::unique_ptr;
using std::shared_ptr;
using std::bind;
using std::placeholders::_1;
using std::map;

// Constructor
// Create a window showing the BA11-L start a thread handling the events
// and render the lamps and switches.
BA11_L::BA11_L (Qbus* bus, Window* window, shared_ptr<BA11_LConfig> ba11lConfig)
    :
    bus_ {bus},
    frontWindow_ {window}
{
    createBezel (ba11lConfig->cabinetPosition);
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
void BA11_L::createBezel (shared_ptr<Cabinet::Position> cabinetPosition)
{
    Panel* panel = frontWindow_->createPanel (cabinetPosition, BA11_LConfig::unitHeight);
    panel->createFront ("../../assets/11_24 front.png", ba11_nFrontFrame);
    powerSwitch_ = panel->createFourPositionSwitch (
        {"../../assets/power_p0.png",
         "../../assets/power_p1.png",
         "../../assets/power_p2.png",
         "../../assets/power_p3.png"},
        Button::FourPositionsState::P0,
        bind (&BA11_L::powerSwitchClicked, this, _1),
        powerSwitchFrame);
    dcOnLed_ = panel->createIndicator ("../../assets/red led off.png",
        "../../assets/red led on.png", Indicator::State::Off, dcOnLedFrame);
}

void BA11_L::powerSwitchClicked (Button::State state)
{
    switch (get<Button::FourPositionsState> (state))
    {
        case Button::FourPositionsState::P0:
            // DC OFF - DC power is removed from the system; contents of
            // MOS memory are lost and system cooling fans are off.
            dcOnLed_->show (Indicator::State::Off);
            // bus_->BPOK ().set (false);
            break;

        case Button::FourPositionsState::P1:
            // LOCAL - Power is applied to the system; enables all functions
            // and normal operation.
            dcOnLed_->show (Indicator::State::On);
            // bus_->BPOK ().set (true);
            break;

        case Button::FourPositionsState::P2:
            // LOC DSBL - Power is present throughout the system. However, the
            // HALT/CONT/BOOT  switch is disabled and the "break" key on the
            // terminal will not halt the CPU.
            break;

        case Button::FourPositionsState::P3:
            // STDBY - DC power to most of the computer is off but dc power is
            // applied to MOS memory to avoid data loss.
            break;
    }
}
