#include "serial_line_units.h"

#include <algorithm>
#include <functional>

using std::bind;
using std::placeholders::_1;

using std::make_unique;
using std::ranges::find_if;

SerialLineUnits::SerialLineUnits (Bus *bus,
    UARTTypeConfig const& uartTypeConfig, SLUConfig* sluConfig)
{
    uarts_[0] = make_unique<UART> (bus, uartTypeConfig,
        sluConfig->uartConfig[0], 0, sluConfig->consoleConfig);
    uarts_[1] = make_unique<UART> (bus, uartTypeConfig,
        sluConfig->uartConfig[1], 1, sluConfig->consoleConfig);
    bus->BINIT().subscribe (bind (&SerialLineUnits::BINITReceiver, this, _1));
}

StatusCode SerialLineUnits::read (BusAddress address, u16 *destination)
{
    auto slu = find_if (uarts_, [address] (auto &uart)
        {return uart->responsible (address);} );
    return (*slu)->read (address, destination);

}

StatusCode SerialLineUnits::writeWord (BusAddress address, u16 value)
{
    auto slu = find_if (uarts_, [address] (auto &uart)
        {return uart->responsible (address);} );
    return (*slu)->writeWord (address, value);
}

bool SerialLineUnits::responsible (BusAddress address)
{
    return uarts_[0]->responsible (address) || uarts_[1]->responsible (address);
}

// On assertion of the BINIT signal initialize the device.
void SerialLineUnits::BINITReceiver (bool signalValue)
{
	if (signalValue)
		reset ();
}

void SerialLineUnits::reset ()
{
    for (unique_ptr<UART>& uart : uarts_)
		uart->reset ();
}