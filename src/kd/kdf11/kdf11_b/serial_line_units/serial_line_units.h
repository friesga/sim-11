#ifndef _SERIALLINEUNITS_H_
#define _SERIALLINEUNITS_H_

#include "qbus/qbus.h"
#include "abstractbusdevice/abstractbusdevice.h"
#include "dl11/uart/uart.h"
#include "configdata/kdf11_b/sluconfig/sluconfig.h"

#include <array>
#include <memory>

using std::array;
using std::unique_ptr;

// This class implements the KDF11-B serial line units.
class SerialLineUnits : public AbstractBusDevice
{
public:
	SerialLineUnits (Qbus *bus, SLUConfig* sluConfig);
    StatusCode read (BusAddress address, u16 *destination);
	StatusCode writeWord (BusAddress address, u16 value);
	bool responsible (BusAddress address);
	void reset ();

private:
    array<unique_ptr<UART>, 2> uarts_;
};


#endif // _SERIALLINEUNITS_H_