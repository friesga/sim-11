#ifndef _BUSSIGNALS_H_
#define _BUSSIGNALS_H_

#include "signal/signal.h"

// This interface declares the functions related to the bus signals. These
// signals have the following functions:
// - SRUN - Indicates that the processor is running,
// - BPOK - Indicates power is applied to the system,
// - RESET - Indicates the system has to be reset,
// - BHALT - Indicates the processor has to halt,
// - START - Indicates the processor has to start running,
// - BINIT - Reset the bus and all devices on the bus,
// - BOOT - Initiate the boot sequence,
// - BatteryPower - Indicates if battery power is available to the system,
// - IOMapEnable - Indicates if the IO map is enabled
//
class BusSignals
{
public:
	virtual Signal& SRUN () = 0;
	virtual Signal& BPOK () = 0;
	virtual Signal& RESET () = 0;
	virtual Signal& BHALT () = 0;
	virtual Signal& START () = 0;
	virtual Signal& BINIT () = 0;
	virtual Signal& BOOT () = 0;
	virtual Signal& BatteryPower () = 0;
	virtual Signal& IOMapEnable () = 0;
};

#endif // _BUSSIGNALS_H_