#ifndef _DLV11CHANNEL_H_
#define _DLV11CHANNEL_H_

#include "qbus/qbus.h"
#include "statuscodes.h"

#include <functional>

using std::function;

class DLV11Channel
{
public:
	DLV11Channel (u16 channelBaseAddress, u16 vector);
	~DLV11Channel ();
	StatusCode read (u16 address, u16 *destAddress);
	StatusCode writeWord (u16 address, u16 value);
	void reset ();

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

private:
	void readChannel ();
	void writeRCSR (u16 value);
	void writeXCSR (u16 value);
	void writeChannel ();
	void receive (unsigned char c);
};

#endif _DLV11CHANNEL_H_