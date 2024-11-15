#include "ba11l.h"

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
BA11_L::BA11_L (Qbus* bus, Window* window, shared_ptr<BA11LConfig> ba11lConfig)
    :
    bus_ {bus},
    frontWindow_ {window}
{
    createBezel (ba11lConfig->cabinetPosition);
}

// Destructor
BA11L::~BA11L ()
{}

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
    Panel* panel = frontWindow_->createPanel (cabinetPosition, BA11LConfig::unitHeight);
}
