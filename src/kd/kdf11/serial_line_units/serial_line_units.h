#ifndef _SERIALLINEUNITS_H_
#define _SERIALLINEUNITS_H_

#include "qbus/qbus.h"
#include "abstractbusdevice/abstractbusdevice.h"
#include "dl11/uart/uart.h"
#include "configdata/kdf11/slu/sluconfig/sluconfig.h"
#include "configdata/serialconfig/uarttypeconfig/uarttypeconfig.h"

#include <array>
#include <memory>

using std::array;
using std::unique_ptr;

// This class implements the KDF11-B / KDF11-U serial line units.
class SerialLineUnits : public AbstractBusDevice
{
public:
	SerialLineUnits (Qbus *bus, UARTTypeConfig const & uartTypeConfig, SLUConfig* sluConfig);
    StatusCode read (BusAddress address, u16 *destination);
	StatusCode writeWord (BusAddress address, u16 value);
	bool responsible (BusAddress address);
	void reset ();
	void BINITReceiver (bool signalValue);

private:
    array<unique_ptr<UART>, 2> uarts_;
};


#endif // _SERIALLINEUNITS_H_