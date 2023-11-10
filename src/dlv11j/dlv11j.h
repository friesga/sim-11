#ifndef _DLV11J_H_
#define _DLV11J_H_

#include "qbus/qbus.h"
#include "pdp11peripheral/pdp11peripheral.h"
#include "configdata/dlv11config/dlv11config.h"
#include "console/console.h"
#include "dlv11channel/dlv11channel.h"

#include <memory>
#include <functional>

using std::unique_ptr;
using std::shared_ptr;
using std::function;

class DLV11J : public PDP11Peripheral
{
public:
	DLV11J (Qbus *bus, shared_ptr<DLV11Config> dlv11Config);
	~DLV11J ();

	// Define the obligatory functions
	StatusCode read (u16 address, u16 *destAddress) override;
	StatusCode writeWord (u16 address, u16 value) override;
	bool responsible (u16 address) override;
	void reset ();

	// Declare the signal receivers
	void BINITReceiver (bool signalValue);

	// Receive a character from the outside world
	void receive (int channel, unsigned char c);

private:
	enum {numChannels = 4};
	enum {defaultBaseAddress_ = 0176500};
	enum {defaultBaseVector_ = 0300};
	enum {defaultCh3Address_ = 0177560};
	enum {defaultCh3Vector_ = 060};

	unique_ptr<DLV11Channel> channel_[4];
	u16	baseAddress_;
	u16 baseVector_;
	shared_ptr<DLV11Config> dlv11Config_;
	DLV11Config::Ch3BreakResponse ch3BreakResponse_;
    unsigned char breakKey_;
	unique_ptr<Console> console_;

	void initialize (bool ch3Enabled);
	void readChannel (int channelNr);
	void writeChannel (int channelNr);
	void writeRCSR (int n, u16 value);
	void writeXCSR (int n, u16 value);
	void console_print (unsigned char c);
	bool queueCharacter (unique_ptr<DLV11Channel>& channel, unsigned char c);
	void receiveDone (unique_ptr<DLV11Channel>& channel);
	void processBreak ();
};

#endif // !_DLV11J_H_
