#ifndef _DLV11CHANNEL_H_
#define _DLV11CHANNEL_H_

#include "qbus/qbus.h"
#include "statuscodes.h"
#include "configdata/dlv11config/dlv11config.h"
#include "console/console.h"

#include <functional>
#include <memory>

using std::function;
using std::unique_ptr;
using std::shared_ptr;

class DLV11Channel
{
public:
	DLV11Channel (Qbus* bus, u16 channelBaseAddress, u16 vector,
		shared_ptr<DLV11Config> dlv11Config);
	~DLV11Channel ();
	StatusCode read (u16 registerAddress, u16 *destAddress);
	StatusCode writeWord (u16 registerAddress, u16 value);
	void reset ();

private:
	// Define DLV11-J's registers as offsets from the controllers base address
    enum
    { 
        RCSR = 00,	// Receiver control/status register
        RBUF = 02,	// Receiver data buffer register
        XCSR = 04,	// Transmitter control/status register
        XBUF = 06	// Transmitter data buffer register
    };

	u16	rcsr {0};
	u16	rbuf {0};
	u16	xcsr {0};
	u16	xbuf {0};

	u16	baseAddress {0};
	u16	vector {0};

	u8*	buf {nullptr};
	u16	buf_r {0};
	u16	buf_w {0};
	u16	buf_size {0};

	Qbus* bus_;
	DLV11Config::Ch3BreakResponse ch3BreakResponse_;
    unsigned char breakKey_;
	u16 channelNr_;
	unique_ptr<Console> console_;

	void readChannel ();
	void writeRCSR (u16 value);
	void writeXCSR (u16 value);
	void writeChannel ();
	void receive (unsigned char c);
	void processBreak ();
	bool queueCharacter (unsigned char c);
	void receiveDone ();
};

#endif _DLV11CHANNEL_H_