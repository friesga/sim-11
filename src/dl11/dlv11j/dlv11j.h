#ifndef _DLV11J_H_
#define _DLV11J_H_

#include "bus/include/bus.h"
#include "abstractbusdevice/abstractbusdevice.h"
#include "configdata/serialconfig/dlv11jconfig/dlv11jconfig.h"
#include "dl11/uart/uart.h"

#include <memory>

using std::unique_ptr;
using std::shared_ptr;

class DLV11J : public AbstractBusDevice
{
public:
	DLV11J (Bus *bus, shared_ptr<DLV11JConfig> dlConfig);

	// Define the obligatory functions
	CondData<u16> read (BusAddress busAddress) override;
	StatusCode writeWord (BusAddress busAddress, u16 value) override;
	bool responsible (BusAddress address) override;
	void reset ();

	// Declare the signal receivers
	void BINITReceiver (bool signalValue);

private:
	enum {numChannels = 4};
	enum {defaultCh3Address_ = 0177560};
	enum {defaultCh3Vector_ = 060};

	unique_ptr<UART> channel_[4];
	u16	baseAddress_;
	u16 baseVector_;
	shared_ptr<DLV11JConfig> dlConfig_;

	// Pointer to the bus we are connected to
	Bus* bus_;

	void initialize ();
	u16 extractChannelNr (BusAddress busAddress);
};

#endif // !_DLV11J_H_
