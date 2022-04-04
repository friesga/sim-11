#include <string.h>
#include <stdlib.h>

#include "trace.h"
#include "dlv11j.h"

#define	RCSR_READER_ENABLE	_BV(0)
#define	RCSR_RCVR_INT		_BV(6)
#define	RCSR_RCVR_DONE		_BV(7)

#define	RBUF_ERROR			_BV(15)
#define	RBUF_OVERRUN		_BV(14)
#define	RBUF_FRAMING_ERROR	_BV(13)
#define	RBUF_PARITY_ERROR	_BV(12)
#define	RBUF_ERROR_MASK		(RBUF_OVERRUN | RBUF_FRAMING_ERROR \
		| RBUF_PARITY_ERROR)

#define	XCSR_TRANSMIT_READY	_BV(7)
#define	XCSR_TRANSMIT_INT	_BV(6)
#define	XCSR_TRANSMIT_BREAK	_BV(0)

#define	RCSR_WR_MASK		(RCSR_RCVR_INT | RCSR_READER_ENABLE)
#define	XCSR_WR_MASK		(XCSR_TRANSMIT_INT | XCSR_TRANSMIT_BREAK)

#define	IRQ(x)			if(!bus->interrupt(x)) irq = (x)

void console_print (unsigned char c)
{
	printf ("%c", c);
	fflush (stdout);
}


DLV11J::DLV11J ()
{
	int i;

	/* Factory configuration */
	base = 0176500;

	memset (channel, 0, sizeof (channel));
	irq = 0;

	for (i = 0; i < 4; i++)
	{
		channel[i].buf = (u8*)malloc (DLV11J_BUF);
		channel[i].buf_r = 0;
		channel[i].buf_w = 0;
		channel[i].buf_size = 0;
		channel[i].base = base + 8 * i;
		channel[i].vector = 300 + 8 * i;
		channel[i].rcsr = 0;
	}

	channel[3].base = 0177560;
	channel[3].vector = 060;
	channel[3].receive = console_print;

	reset ();
}

// The class has a meaningful destructor so the "rule of five" applies
// and copy and move constructors and copy and move assignment operators
// have to be defined.
DLV11J::~DLV11J ()
{
	u8 i;

	for (i = 0; i < 4; i++)
	{
		free (channel[i].buf);
	}
}

void DLV11J::readChannel (int channelNr)
{
	DLV11Ch* ch = &channel[channelNr];
	if(ch->buf_size > 0)
	{
		ch->rbuf = (u8) ch->buf[ch->buf_r++];
		ch->buf_r %= DLV11J_BUF;
		ch->buf_size--;

		if(ch->buf_size) {
			/* more date in the RX buffer... */
			ch->rcsr |= RCSR_RCVR_DONE;
			if(ch->rcsr & RCSR_RCVR_INT) {
				QBUS* bus = this->bus;
				IRQ(ch->vector);
			}
		} else {
			ch->rcsr &= ~RCSR_RCVR_DONE;
		}
	} else {
		ch->rbuf = RBUF_OVERRUN;
		if(ch->rbuf & RBUF_ERROR_MASK) {
			ch->rbuf |= RBUF_ERROR;
		}
	}
}

void DLV11J::writeChannel(int channelNr)
{
	DLV11Ch* ch = &channel[channelNr];
	TRCDLV11(TRC_DLV11_TX, channelNr, ch->xbuf);

	if(ch->receive)
	{
		ch->receive((unsigned char) ch->xbuf);
	}
	ch->xcsr |= XCSR_TRANSMIT_READY;
	if(ch->xcsr & XCSR_TRANSMIT_INT)
	{
		QBUS* bus = this->bus;
		IRQ(ch->vector + 4);
	}
}

u16 DLV11J::read (u16 address)
{
	switch(address)
	{
		case 0177560:
			return channel[3].rcsr;
		case 0177562:
			readChannel(3);
			return channel[3].rbuf;
		case 0177564:
			return channel[3].xcsr;
		case 0177566:
			return channel[3].xbuf;
		default:
			return 0;
	}
}

void DLV11J::writeRCSR(int n, u16 value)
{
	DLV11Ch* ch = &channel[n];
	u16 old = ch->rcsr;
	ch->rcsr = (ch->rcsr & ~RCSR_WR_MASK) | (value & RCSR_WR_MASK);
	
	if((value & RCSR_RCVR_INT) && !(old & RCSR_RCVR_INT)
			&& (ch->rcsr & RCSR_RCVR_DONE))
	{
		QBUS* bus = this->bus;
		IRQ(ch->vector);
	}
}

void DLV11J::writeXCSR(int n, u16 value)
{
	DLV11Ch* ch = &channel[n];
	u16 old = ch->xcsr;
	ch->xcsr = (ch->xcsr & ~XCSR_WR_MASK) | (value & XCSR_WR_MASK);
	
	if((value & XCSR_TRANSMIT_INT) && !(old & XCSR_TRANSMIT_INT)
			&& (ch->xcsr & XCSR_TRANSMIT_READY))
	{
		QBUS* bus = this->bus;
		IRQ(ch->vector + 4);
	}
}

void DLV11J::write (u16 address, u16 value)
{
	switch(address) {
		case 0177560:
			writeRCSR(3, value);
			break;
		case 0177562:
			/* ignored */
			break;
		case 0177564:
			writeXCSR(3, value);
			break;
		case 0177566:
			channel[3].xbuf = value;
			writeChannel(3);
			break;
	}
}

u8 DLV11J::responsible (u16 address)
{
	if(address >= base && address <= base + (3 * 8)) {
		return 1;
	}

	/* console device */
	if(address >= 0177560 && address <= 0177566) {
		return 1;
	}

	return 0;
}

void DLV11J::reset ()
{
	u8 i;

	irq = 0;

	for(i = 0; i < 4; i++) {
		channel[i].rcsr &= ~RCSR_RCVR_INT;
		channel[i].xcsr = XCSR_TRANSMIT_READY;
	}
}

void DLV11J::send(int channelNr, unsigned char c)
{
	DLV11Ch* ch = &channel[channelNr];
	if(ch->buf_size < DLV11J_BUF)
	{
		TRCDLV11(TRC_DLV11_RX, channelNr, c);
		ch->buf[ch->buf_w++] = c;
		ch->buf_w %= DLV11J_BUF;
		ch->buf_size++;
		ch->rcsr |= RCSR_RCVR_DONE;
		if(ch->rcsr & RCSR_RCVR_INT)
		{
			QBUS* bus = this->bus;
			IRQ(ch->vector);
		}
	}
}

void DLV11J::step()
{
	if(irq)
	{
		QBUS* bus = this->bus;
		if(bus->interrupt(irq))
			irq = 0;
	}
}
