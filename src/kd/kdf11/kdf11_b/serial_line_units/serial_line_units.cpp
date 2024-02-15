#include "serial_line_units.h"

#include <algorithm>

using std::make_unique;
using std::ranges::find_if;

SerialLineUnits::SerialLineUnits (Qbus *bus, SLUConfig* sluConfig)
{
    uarts_[0] = make_unique<UART> (bus, sluConfig->uartConfig[0], 0, sluConfig->consoleConfig);
    uarts_[1] = make_unique<UART> (bus, sluConfig->uartConfig[1], 1, sluConfig->consoleConfig);
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

void SerialLineUnits::reset ()
{}