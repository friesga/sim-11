#ifndef _SIGNALHANDLER_H_
#define _SIGNALHANDLER_H_

#include "bus/include/bussignals.h"

// The Qbus contains a set of control signals that can be set, cycled
// and tested.
//
// The SRUN L signal is actually not a bus signal, but a non-bused, backplane
// signal. The signal is a series of pulses which occur at 3-5" intervals
// whenever the processor is in the Run mode.
//
// The BPOK signal is triggered:
// - On power up or,
// - When the console BREAK key is hit and the boot response is configured.
// On assertion of this signal the processor executes the power up routine.
// 
// The PDP Bus Handbook states that "BOCOK H may be pulsed low for a minimum
// of 1 microsecond to cause the CPU to restart" while BPOK is high. This
// pulse of BDCOK is replaced by a separate RESET signal.
// 
// The BINIT signal is triggered:
// - When the processor executes the power up routine,
// - On a RESET instruction,
// - On a ODT Go command.
// On assertion of this signal all bus devices - including the bus itself -
// are initialized.
// 
// These signals are (or will become) abstraction of the actual bus
// Unibus or Qbus signals and not every system will contain all of these
// signals. These signals are generic and a system can take use of it to
// provide specific functionality. The 11/24 e.g. uses the BOOT signal
// which is not used by the 11/03 and 11/23 systems.
//
class SignalHandler
{
public:
	Signal& SRUN ();
	Signal& BPOK ();
	Signal& RESET ();
	Signal& BHALT ();
	Signal& BINIT ();
	Signal& BOOT ();
	Signal& BatteryPower ();
	Signal& IOMapEnable ();

private:
	// Definition of the bus signals. The BatteryPower signal is present
	// only on systems with battery backup. The initial value of the
	// BatteryPower signal is false and in systems without battery backup
	// that signal remains false.
	//
	Signal SRUN_;
	Signal BPOK_;
	Signal RESET_;
	Signal BHALT_;
	Signal BINIT_;
	Signal BOOT_;
	Signal BatteryPower_;
	Signal IOMapEnable_;
};

#endif _SIGNALHANDLER_H_
