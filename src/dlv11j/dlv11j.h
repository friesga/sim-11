#ifndef _DLV11J_H_
#define _DLV11J_H_

#include "qbus/qbus.h"
#include "busdevice/busdevice.h"
#include "configdata/dlv11config/dlv11config.h"
#include "console/console.h"

#include <memory>
#include <functional>

using std::unique_ptr;
using std::shared_ptr;
using std::function;

/* DLV11-J input buffer */
#define	DLV11J_BUF		2048

struct DLV11Ch
{
	u16	rcsr {0};
	u16	rbuf {0};
	u16	xcsr {0};
	u16	xbuf {0};

	u16	base {0};
	u16	vector {0};

	u8*	buf {nullptr};
	u16	buf_r {0};
	u16	buf_w {0};
	u16	buf_size {0};

	// Send a character from the DLV11-J to the outside world.
	// void (*send)(unsigned char c);
	function<void(unsigned char)> send {};
};

class DLV11J : public BusDevice
{
public:
	DLV11J (Qbus *bus, unique_ptr<Console> console);
	DLV11J (Qbus *bus, unique_ptr<Console> console, 
		shared_ptr<DLV11Config> dlv11Config);
	~DLV11J ();

	// Define the obligatory functions
	StatusCode read (u16 address, u16 *destAddress) override;
	StatusCode writeWord (u16 address, u16 value) override;
	bool responsible (u16 address) override;
	void reset ();

	// Receive a character from the outside world
	void receive (int channel, unsigned char c);

private:
	enum {numChannels = 4};
	enum {defaultBaseAddress_ = 0176500};
	enum {defaultBaseVector_ = 0300};
	enum {defaultCh3Address_ = 0177560};
	enum {defaultCh3Vector_ = 060};

	DLV11Ch	channel_[4];
	u16	baseAddress_;
	u16 baseVector_;
	DLV11Config::Ch3BreakResponse ch3BreakResponse_;
    unsigned char breakKey_;
	unique_ptr<Console> console_;

	void initializeChannels ();
	void readChannel (int channelNr);
	void writeChannel (int channelNr);
	void writeRCSR (int n, u16 value);
	void writeXCSR (int n, u16 value);
	void console_print (unsigned char c);
};

#endif // !_DLV11J_H_
