#include "kw11l.h"
#include "chrono/simulatorclock/simulatorclock.h"
#include "chrono/alarmclock/alarmclock.h"

#include <chrono>

using std::chrono::system_clock;
using namespace std::chrono;

KW11L::KW11L (Bus* bus, const KW11LConfig& kw11lConfig)
    :
	bus_ {bus},
    clockSource_ {kw11lConfig.clockSource},
	running_ {true}
{
	reset ();
	ltcThread_ = thread (&KW11L::tick, this);
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
// only be cleared by writing a zero to it. Diagnostic ZKWAG0 test 23 and
// 27 indicate that the second statement is the correct one, trying to
// set the bit should not affect it.
//
StatusCode KW11L::writeWord (BusAddress address, u16 value)
{
	// Guard against simultaneous register updates
	std::lock_guard<std::mutex> guard {kw11lMutex_};

    STATUSREGISTER newRegisterValue {value};
    statusRegister_.interruptEnable = newRegisterValue.interruptEnable;

	if (newRegisterValue.interruptMonitor == 0)
		statusRegister_.interruptMonitor = 0;

	if (!statusRegister_.interruptEnable)
		// Clear possibly pending interrupts
        bus_->clearInterrupt (InterruptPriority::BR6, 9, 0);
    
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
	// Guard against simultaneous register updates
	std::lock_guard<std::mutex> guard {kw11lMutex_};

    statusRegister_.value = 0;
    statusRegister_.interruptMonitor = 1;

	// Clear possibly pending interrupts
	bus_->clearInterrupt (InterruptPriority::BR6, 9, 0);
}

// The KW11-L's priority level is hardwired to BR6 ((EK-KW11L-TM-002, p 2-2).
// It's vector address is hardwired to address 0100.
//
// There are two ways to calculate the cycle time, using the internal
// SimulatorClock or using the system clock. These two deviate significantly
// from each other, depending on the host performance, compiler options
// used and the load on the host. The SimulatorClock should be used if
// diagnostic software uses the line time clock for timing purposes; in
// all other cases use of the system clock is recommended as that provides
// a more accurate operating system time.
//
void KW11L::tick ()
{

	if (clockSource_ == KW11LConfig::ClockSource::SimulatorClock)
	{
		AlarmClock alarmClock {};
		SimulatorClock::time_point nextWakeup {SimulatorClock::now ()};
		constexpr SimulatorClock::duration cycleTime
		{std::chrono::microseconds {1'000'000 / TICK_RATE}};

		while (running_)
		{
			// Guard against simultaneous register updates
			std::unique_lock<std::mutex> guard {kw11lMutex_};

			// Set monitor bit this cycle
			statusRegister_.interruptMonitor = 1;

			// Check the line time clock (LTC) is enabled
			if (statusRegister_.interruptEnable)
				bus_->requestInterrupt (InterruptPriority::BR6, 9, 0, vectorAddress);

			guard.unlock ();
			nextWakeup += cycleTime;
			alarmClock.sleepUntil (nextWakeup);
		}
	}
	else
	{
		system_clock::time_point nextWakeup = system_clock::now ();
		system_clock::duration const cycleTime {
			std::chrono::microseconds {1'000'000 / TICK_RATE}};

		while (running_)
		{
			// Guard against simultaneous register updates
			std::unique_lock<std::mutex> guard {kw11lMutex_};

			// Set monitor bit this cycle
			statusRegister_.interruptMonitor = 1;

			if (statusRegister_.interruptEnable)
				bus_->requestInterrupt (InterruptPriority::BR6, 9, 0, vectorAddress);

			guard.unlock ();
			nextWakeup += cycleTime;
			std::this_thread::sleep_until (nextWakeup);
		}
	}
}