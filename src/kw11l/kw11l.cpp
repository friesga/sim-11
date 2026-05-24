#include "kw11l.h"

#include <chrono>

using std::chrono::system_clock;

// The KW11-L has no options so the reference to KW11LConfig is included just
// for the form's sake.
KW11L::KW11L (Bus* bus, const KW11LConfig& kw11lConfig)
    :
	bus_ {bus},
    ltcThread_ {thread (&KW11L::tick, this)},
	running_ {true}
{
	reset ();
}

KW11L::~KW11L ()
{
    running_ = false;
    ltcThread_.join ();
}

CondData<u16> KW11L::read (BusAddress address)
{
    return statusRegister_.value;
}

// Bit 7 is [...] cleared by any processor DATO to the KW11-L.
// (EK-KW11L-TM-002 p 2-2.) This implies bit 7 is cleared regardless of the
// given new value. Page 3-3 of that same manual however states: "DATO and
// ADDRESS clear D07 when BUS D07 is true". This would imply that bit 7 can
// only be cleared by writing a zero to it. As that is used in the example
// code in chapter 4 we presume that is the actual behaviour of the device.
//
StatusCode KW11L::writeWord (BusAddress address, u16 value)
{
    STATUSREGISTER newRegisterValue {value};
    statusRegister_.interruptEnable = newRegisterValue.interruptEnable;
    
	if (newRegisterValue.interruptMonitor == 0)
		statusRegister_.interruptMonitor = 0;
    
	return StatusCode::Success;
}

bool KW11L::responsible (BusAddress address)
{
    return address.isInIOpage () &&
        address.registerAddress () == statusRegisterAddress;
}

// Bit 7 is set by a processor INIT (EK-KW11L-TM-002, p 2-2.)
void KW11L::reset ()
{
    statusRegister_.value = 0;
    statusRegister_.interruptMonitor = 1;
}

// The KW11-L's priority level is hardwired to BR6 ((EK-KW11L-TM-002, p 2-2).
// It's vesctor address is hardwired to address 0100.
void KW11L::tick ()
{
	system_clock::time_point nextWakeup = system_clock::now ();
	system_clock::duration const cycleTime {
		std::chrono::microseconds {1'000'000 / LTC_RATE}};

	while (running_)
	{
		// Set monitor bit this cycle
		statusRegister_.interruptMonitor = 1;

		// Check the line time clock (LTC) is enabled
		if (statusRegister_.interruptEnable)
			bus_->requestInterrupt (InterruptPriority::BR6, 9, 0, vectorAddress);
		else
			// Clear possibly pending interrupts
			bus_->clearInterrupt (InterruptPriority::BR6, 9, 0);

		nextWakeup += cycleTime;
		std::this_thread::sleep_until (nextWakeup);
	}
}