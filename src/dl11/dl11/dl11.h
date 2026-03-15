#ifndef _DL11_H_
#define _DL11_H_

#include "bus/include/bus.h"
#include "abstractbusdevice/abstractbusdevice.h"
#include "configdata/serialconfig/dl11config/dl11config.h"
#include "dl11/uart/uart.h"

class DL11 : public AbstractBusDevice
{
public:
	DL11 (Bus *bus, const DL11Config& dlConfig);

	// Define the obligatory functions
	CondData<u16> read (BusAddress busAddress) override;
	StatusCode writeWord (BusAddress busAddress, u16 value) override;
	bool responsible (BusAddress address) override;
	void reset () override;

	// Declare the signal receivers
	void BINITReceiver (bool signalValue);

private:
	// Pointer to the bus we are connected to
	Bus* bus_;

	unique_ptr<UART> uart_;
	u16	baseAddress_;
	u16 baseVector_;
	const DL11Config& dlConfig_;

	void initialize ();
};

#endif // !_DL11_H_
