#ifndef _DLV11J_H_
#define _DLV11J_H_

#include "qbus/qbus.h"
#include "busdevice/busdevice.h"
#include "configdata/dlv11config/dlv11config.h"

using std::shared_ptr;

/* DLV11-J input buffer */
#define	DLV11J_BUF		2048

struct DLV11Ch
{
	u16	rcsr;
	u16	rbuf;
	u16	xcsr;
	u16	xbuf;

	u16	base;
	u16	vector;

	u8*	buf;
	u16	buf_r;
	u16	buf_w;
	u16	buf_size;

	void (*receive)(unsigned char c);
};

class DLV11J : public BusDevice
{
public:
	DLV11J (Qbus *bus);
	DLV11J (Qbus *bus, shared_ptr<DLV11Config> dlv11Config);
	~DLV11J ();

	// Define the obligatory functions
	StatusCode read (u16 address, u16 *destAddress) override;
	StatusCode writeWord (u16 address, u16 value) override;
	bool responsible (u16 address) override;
	void reset ();

	void send (int channel, unsigned char c);

private:
	enum {defaultBaseAddress_ = 0176500};
	enum {defaultBaseVector_ = 0300};
	enum {defaultCh3Address_ = 0177560};
	enum {defaultCh3Vector_ = 060};

	DLV11Ch	channel_[4];
	u16	baseAddress_;
	u16 baseVector_;
	DLV11Config::Ch3BreakResponse ch3BreakResponse_;
    unsigned char breakKey_;

	void initializeChannels ();
	void readChannel (int channelNr);
	void writeChannel (int channelNr);
	void writeRCSR (int n, u16 value);
	void writeXCSR (int n, u16 value);
};

#endif // !_DLV11J_H_
