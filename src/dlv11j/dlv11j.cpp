#include <string.h>
#include <stdlib.h>

#include "trace/trace.h"
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

// Print a character from the DLV11-J to the outside world (i.e. the
// console in this case).
void console_print (unsigned char c)
{
	// Just print 7-bit ASCII characters. The XXDP diagnostic VKABB0 e.g.
	// prints characters 0377 at the end of a string which stops WSL from
	// outputting further characters.
	printf ("%c", c & 0177);
	fflush (stdout);
}

// Construct a default DLV11-J object, i.e. without a user-specified
// configuration.
DLV11J::DLV11J (Qbus *bus)
	:
	BusDevice (bus),
	base {0176500},		// Factory configuration
	ch3BreakResponse_ {DLV11Config::Ch3BreakResponse::None},
	breakKey_ {0}
{
	memset (channel, 0, sizeof (channel));

	int channelNr;
	for (channelNr = 0; channelNr < 4; ++channelNr)
	{
		channel[channelNr].buf = (u8*) malloc (DLV11J_BUF);
		channel[channelNr].buf_r = 0;
		channel[channelNr].buf_w = 0;
		channel[channelNr].buf_size = 0;
		channel[channelNr].base = base + 8 * channelNr;
		channel[channelNr].vector = 300 + 8 * channelNr;
		channel[channelNr].rcsr = 0;
	}

	channel[3].base = 0177560;
	channel[3].vector = 060;
	channel[3].receive = console_print;

	reset ();
}

// Construct a DLV11-J object with the configuration as specified by
// the user.
DLV11J::DLV11J (Qbus *bus, shared_ptr<DLV11Config> dlv11Config)
	:
	DLV11J (bus)
{
	ch3BreakResponse_ = dlv11Config->ch3BreakResponse;
	breakKey_ = dlv11Config->breakKey;
}

// The class has a meaningful destructor so the "rule of five" applies
// and copy and move constructors and copy and move assignment operators
// have to be defined.
DLV11J::~DLV11J ()
{
	u8 channelNr;

	for (channelNr = 0; channelNr < 4; ++channelNr)
	{
		free (channel[channelNr].buf);
	}
}

void DLV11J::readChannel (int channelNr)
{
	DLV11Ch* ch = &channel[channelNr];
	if (ch->buf_size > 0)
	{
		ch->rbuf = (u8) ch->buf[ch->buf_r++];
		ch->buf_r %= DLV11J_BUF;
		ch->buf_size--;

		if(ch->buf_size)
		{
			/* more date in the RX buffer... */
			ch->rcsr |= RCSR_RCVR_DONE;
			if(ch->rcsr & RCSR_RCVR_INT)
				bus_->setInterrupt (TrapPriority::BR4, 6, ch->vector);
		} 
		else
			ch->rcsr &= ~RCSR_RCVR_DONE;
	} 
	else
	{
		ch->rbuf = RBUF_OVERRUN;
		if (ch->rbuf & RBUF_ERROR_MASK)
			ch->rbuf |= RBUF_ERROR;
	}
}

void DLV11J::writeChannel (int channelNr)
{
	DLV11Ch* ch = &channel[channelNr];
	trace.dlv11 (DLV11RecordType::DLV11_TX, channelNr, ch->xbuf);

	if (ch->receive)
		ch->receive ((unsigned char) ch->xbuf);
	
	ch->xcsr |= XCSR_TRANSMIT_READY;
	if (ch->xcsr & XCSR_TRANSMIT_INT)
		bus_->setInterrupt (TrapPriority::BR4, 6, ch->vector + 4);
}

// This function allows the host system to read a word from one of the
// DLV11-J's registers.
StatusCode DLV11J::read (u16 address, u16 *destAddress)
{
	switch (address)
	{
		case 0177560:
			*destAddress = channel[3].rcsr;
			break;

		case 0177562:
			readChannel(3);
			*destAddress = channel[3].rbuf;
			break;

		case 0177564:
			*destAddress = channel[3].xcsr;
			break;

		case 0177566:
			*destAddress = channel[3].xbuf;
			break;

		default:
			return StatusCode::NonExistingMemory;
	}
	return StatusCode::OK;
}

void DLV11J::writeRCSR (int n, u16 value)
{
	DLV11Ch* ch = &channel[n];
	u16 old = ch->rcsr;
	ch->rcsr = (ch->rcsr & ~RCSR_WR_MASK) | (value & RCSR_WR_MASK);
	
	if ((value & RCSR_RCVR_INT) && !(old & RCSR_RCVR_INT)
			&& (ch->rcsr & RCSR_RCVR_DONE))
		bus_->setInterrupt (TrapPriority::BR4, 6, ch->vector);
}

void DLV11J::writeXCSR (int n, u16 value)
{
	DLV11Ch* ch = &channel[n];
	u16 old = ch->xcsr;
	ch->xcsr = (ch->xcsr & ~XCSR_WR_MASK) | (value & XCSR_WR_MASK);
	
	if ((value & XCSR_TRANSMIT_INT) && !(old & XCSR_TRANSMIT_INT)
			&& (ch->xcsr & XCSR_TRANSMIT_READY))
		bus_->setInterrupt (TrapPriority::BR4, 6, ch->vector + 4);
}

// This function allows the host system to wriite a word to one of the
// DLV11-J's registers.
StatusCode DLV11J::writeWord (u16 address, u16 value)
{
	switch (address)
	{
		case 0177560:
			writeRCSR (3, value);
			break;

		case 0177562:
			/* ignored */
			break;

		case 0177564:
			writeXCSR (3, value);
			break;

		case 0177566:
			channel[3].xbuf = value;
			writeChannel (3);
			break;
	}

	return StatusCode::OK;
}

bool DLV11J::responsible (u16 address)
{
	if (address >= base && address <= base + (3 * 8))
		return true;

	/* console device */
	if (address >= 0177560 && address <= 0177566)
		return true;

	return false;
}

void DLV11J::reset ()
{
	u8 channelNr;

	for (channelNr = 0; channelNr < 4; ++channelNr)
	{
		channel[channelNr].rcsr &= ~RCSR_RCVR_INT;
		channel[channelNr].xcsr = XCSR_TRANSMIT_READY;
	}
}

// Send a character from outside the system to the DLV11-J
void DLV11J::send (int channelNr, unsigned char c)
{
	DLV11Ch* ch = &channel[channelNr];
	if(ch->buf_size < DLV11J_BUF)
	{
		trace.dlv11 (DLV11RecordType::DLV11_RX, channelNr, c);
		ch->buf[ch->buf_w++] = c;
		ch->buf_w %= DLV11J_BUF;
		ch->buf_size++;
		ch->rcsr |= RCSR_RCVR_DONE;
		if (ch->rcsr & RCSR_RCVR_INT)
			bus_->setInterrupt (TrapPriority::BR4, 6, ch->vector);
	}
}

