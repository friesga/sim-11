#ifndef _DLV11J_H_
#define _DLV11J_H_

#include "qbus/qbus.h"

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
	void	(*receive)(unsigned char c);
};

class DLV11J : public QBUSModule
{
public:
	DLV11J ();
	~DLV11J ();

	// Define the obligatory functions
	StatusCode read (u16 address, u16 *destAddress);
	void writeWord (u16 address, u16 value);
	u8 responsible (u16 address);
	void reset ();

	void send (int channel, unsigned char c);
	void step();

private:
	void readChannel (int channelNr);
	void writeChannel (int channelNr);
	void writeRCSR (int n, u16 value);
	void writeXCSR (int n, u16 value);
	InterruptRequest interruptRequest(unsigned char vector);

	DLV11Ch	channel[4];
	u16	base;
};

#endif // !_DLV11J_H_
