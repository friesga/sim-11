#include "trace/trace.h"
#include "dl11.h"
#include "configdata/serialconfig/uarttypeconfig/uarttypeconfig.h"
#include "configdata/serialconfig/consoleconfig/consoleconfig.h"

#include <string.h>
#include <stdlib.h>
#include <functional>
#include <ranges>

using std::bind;
using std::placeholders::_1;
using std::make_unique;
using std::ranges::any_of;

// Construct a DL11 object with the given configuration
DL11::DL11 (Bus *bus, const DL11Config& dlConfig)
	:
	bus_ {bus},
	baseAddress_ {dlConfig.baseAddress},
	baseVector_ {dlConfig.baseVector},
	dlConfig_ {dlConfig}
{
	initialize ();

	reset ();
}

// Initialize the DLV11J and its channels
// 
// If channel 3 is to be used as console the channel's base address
// and vector have to be set to the appropriate values.
//
// When channel 3 is configured as the console device interface [...]
// the interrupt vectors of the channel become 60 and 64. This is true
// regardless of the configured base vector of the module.
// (EK-DLV1J-UG-001)
//
void DL11::initialize ()
{
	uart_ = make_unique<UART> (bus_,
			UARTTypeConfig {.maintenanceModeSupported = true},
		dlConfig_.uart, 0, ConsoleConfig {ConsoleConfig::BreakResponse::None, 0});

	bus_->BINIT().subscribe (bind (&DL11::BINITReceiver, this, _1));
}

// This function allows the host system to read a word from one of the
// DLV11-J's registers. The read command is passed through to the appropriate
// channel.
CondData<u16> DL11::read (BusAddress busAddress)
{
	return uart_->read (busAddress);
}

// This function allows the host system to write a word to one of the
// DLV11-J's registers. The write command is passed through to the appropriate
// channel.
StatusCode DL11::writeWord (BusAddress busAddress, u16 value)
{
	return uart_->writeWord (busAddress, value);
}

// The responsibility for the handling of bus addresses is delegated to the
// DLV11-J's channels.
bool DL11::responsible (BusAddress busAddress)
{
	if (!busAddress.isInIOpage ())
		return false;

	return uart_->responsible (busAddress);
}

// On assertion of the BINIT signal initialize the device.
void DL11::BINITReceiver (bool signalValue)
{
	if (signalValue)
		reset ();
}

void DL11::reset ()
{
	uart_->reset ();
}


