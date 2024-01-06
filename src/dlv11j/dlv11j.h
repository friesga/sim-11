#ifndef _DLV11J_H_
#define _DLV11J_H_

#include "qbus/qbus.h"
#include "pdp11peripheral/pdp11peripheral.h"
#include "configdata/dlv11config/dlv11config.h"
#include "dlv11channel/dlv11channel.h"

#include <memory>

using std::unique_ptr;
using std::shared_ptr;

class DLV11J : public PDP11Peripheral
{
public:
	DLV11J (Qbus *bus, shared_ptr<DLV11Config> dlv11Config);

	// Define the obligatory functions
	StatusCode read (BusAddress registerAddress, u16 *destAddress) override;
	StatusCode writeWord (BusAddress registerAddress, u16 value) override;
	bool responsible (BusAddress address) override;
	void reset ();

	// Declare the signal receivers
	void BINITReceiver (bool signalValue);

private:
	enum {numChannels = 4};
	enum {defaultCh3Address_ = 0177560};
	enum {defaultCh3Vector_ = 060};

	unique_ptr<DLV11Channel> channel_[4];
	u16	baseAddress_;
	u16 baseVector_;
	shared_ptr<DLV11Config> dlv11Config_;

	void initialize (bool ch3Enabled);
	u16 extractChannelNr (BusAddress busAddress);
};

#endif // !_DLV11J_H_
