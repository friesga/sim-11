#include "dlv11channel.h"
#include "trace/trace.h"
#include "console/operatorconsole/operatorconsolefactory.h"

#include <memory>
#include <functional>

using std::bind;
using std::placeholders::_1;

/* Size of DLV11-J input buffer */
#define	DLV11J_BUF		2048

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


DLV11Channel::DLV11Channel (Qbus* bus, u16 channelBaseAddress, 
	u16 channelVector, shared_ptr<DLV11Config> dlv11Config)
	:
	buf {(u8*) malloc (DLV11J_BUF)},
	baseAddress {channelBaseAddress},
	vector {channelVector},
	bus_ {bus},
	ch3BreakResponse_ {dlv11Config->ch3BreakResponse},
	breakKey_ {dlv11Config->breakKey}
{
	// Determine the channel number from the base address. An exception
	// to the standard formula has to be made when channel 3 is used as
	// a console device.
	if (baseAddress == 0177560)
		channelNr_ = 3;
	else
		channelNr_ = static_cast<u16> ((baseAddress & 030) >> 3);

	if (channelNr_ == 3)
	{
		console_ = OperatorConsoleFactory::create ();

		// Pass the console the function we want to receive the characters on
		console_->setReceiver (bind (&DLV11Channel::receive, this, _1));
	}
	
	reset ();
}

DLV11Channel::~DLV11Channel ()
{
	free (buf);
}

void DLV11Channel::reset ()
{
	rcsr &= ~RCSR_RCVR_INT;
	xcsr = XCSR_TRANSMIT_READY;
}

// This function allows the host system to read a word from one of the
// DLV11-J's registers.
StatusCode DLV11Channel::read (u16 registerAddress, u16 *destAddress)
{
	switch (registerAddress & 06)
	{
		case RCSR:
			*destAddress = rcsr;
			break;

		case RBUF:
			readChannel();
			*destAddress = rbuf;
			break;

		case XCSR:
			*destAddress = xcsr;
			break;

		case XBUF:
			*destAddress = xbuf;
			break;

		default:
			return StatusCode::NonExistingMemory;
	}
	return StatusCode::OK;
}

void DLV11Channel::readChannel ()
{
	if (buf_size > 0)
	{
		rbuf = (u8) buf[buf_r++];
		buf_r %= DLV11J_BUF;
		buf_size--;

		if (buf_size)
		{
			// More date in the RX buffer...
			rcsr |= RCSR_RCVR_DONE;
			if (rcsr & RCSR_RCVR_INT)
				bus_->setInterrupt (TrapPriority::BR4, 6, vector);
		} 
		else
			rcsr &= ~RCSR_RCVR_DONE;
	} 
	else
	{
		rbuf = RBUF_OVERRUN;
		if (rbuf & RBUF_ERROR_MASK)
			rbuf |= RBUF_ERROR;
	}
}

StatusCode DLV11Channel::writeWord (u16 registerAddress, u16 value)
{
	switch (registerAddress & 06)
	{
		case RCSR:
			writeRCSR (value);
			break;

		case RBUF:
			// ignored
			break;

		case XCSR:
			writeXCSR (value);
			break;

		case XBUF:
			xbuf = value;
			writeChannel ();
			break;
	}

	return StatusCode::OK;
}

void DLV11Channel::writeRCSR (u16 value)
{
	u16 old = rcsr;
	rcsr = (rcsr & ~RCSR_WR_MASK) | (value & RCSR_WR_MASK);
	
	if ((value & RCSR_RCVR_INT) && !(old & RCSR_RCVR_INT)
			&& (rcsr & RCSR_RCVR_DONE))
		bus_->setInterrupt (TrapPriority::BR4, 6, vector);
}

void DLV11Channel::writeXCSR (u16 value)
{
	u16 old = xcsr;
	xcsr = (xcsr & ~XCSR_WR_MASK) | (value & XCSR_WR_MASK);
	
	if ((value & XCSR_TRANSMIT_INT) && !(old & XCSR_TRANSMIT_INT)
			&& (xcsr & XCSR_TRANSMIT_READY))
		bus_->setInterrupt (TrapPriority::BR4, 6, vector + 4);
}

void DLV11Channel::writeChannel ()
{
	trace.dlv11 (DLV11RecordType::DLV11_TX, channelNr_, xbuf);

	if (console_)
		console_->print ((unsigned char) xbuf);
	
	xcsr |= XCSR_TRANSMIT_READY;
	if (xcsr & XCSR_TRANSMIT_INT)
		bus_->setInterrupt (TrapPriority::BR4, 6, vector + 4);
}

// The functions above interface with the processor; the following functions
// interface with the Console.
//
// Receive a character from outside the system in the DLV11-J. If the
// system is powered on the received character is processed, else it is
// ignored.
void DLV11Channel::receive (unsigned char c)
{
	if (bus_->BPOK ())
	{
		if (baseAddress == 0177560 && c == breakKey_)
		{
			// Process the BREAK, not queueing it as a received character
			processBreak ();
			return;
		}

		if (queueCharacter (c))
		{
			trace.dlv11 (DLV11RecordType::DLV11_RX, channelNr_, c);
			receiveDone ();
		}
	}
}

// Hitting the BREAK key on the console initiates a Channel 3 Break Reponse.
// The response is either cycling the BHALT signal, cycling the RESET signal
// or a no-operation.
// 
// Cycling the BHALT signal results in halting the processor and cycling the
// RESET signal results in the execution of the power up routine. The signals
// are cycled as the key presses have to be treated as triggers.
// 
// As we don't have a real BREAK key at our disposal and a BREAK key press
// is replaced by a (configurable) regular key press, the no-operation
// response has to result in passing the received character on to the host
// system.
void DLV11Channel::processBreak ()
{
	if (ch3BreakResponse_ == DLV11Config::Ch3BreakResponse::Halt)
	{
		bus_->BHALT ().cycle ();
		return;
	}
	else if (ch3BreakResponse_ == DLV11Config::Ch3BreakResponse::Boot)
	{
		bus_->RESET ().cycle ();
		return;
	}
}

// Put the given character in the buffer for the given channel. The buffer
// is a queue implemented as a fixed-size circular array. The array contains
// buf_size characters with the head at position buf_w.
bool DLV11Channel::queueCharacter (unsigned char c)
{
	if (buf_size < DLV11J_BUF)
	{
		buf[buf_w++] = c;
		buf_w %= DLV11J_BUF;
		buf_size++;
		return true;
	}

	return false;
}

void DLV11Channel::receiveDone ()
{
	rcsr |= RCSR_RCVR_DONE;
	if (rcsr & RCSR_RCVR_INT)
		bus_->setInterrupt (TrapPriority::BR4, 6, vector);
}