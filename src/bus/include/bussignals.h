#ifndef _BUSSIGNALS_H_
#define _BUSSIGNALS_H_

#include "signal/signal.h"

// This interface declares the functions related to the bus signals.
class BusSignals
{
public:
	virtual Signal& SRUN () = 0;
	virtual Signal& BPOK () = 0;
	virtual Signal& RESET () = 0;
	virtual Signal& BHALT () = 0;
	virtual Signal& BINIT () = 0;
	virtual Signal& BOOT () = 0;
	virtual Signal& BatteryPower () = 0;
	virtual Signal& IOMapEnable () = 0;
};

#endif // _BUSSIGNALS_H_