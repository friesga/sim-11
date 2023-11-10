#include "trace/trace.h"
#include "dlv11j.h"
#include "console/operatorconsole/operatorconsolefactory.h"

#include <string.h>
#include <stdlib.h>
#include <functional>

using std::bind;
using std::placeholders::_1;
using std::placeholders::_2;
using std::make_unique;

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

// Construct a DLV11-J object with the given configuration
DLV11J::DLV11J (Qbus *bus, shared_ptr<DLV11Config> dlv11Config)
	:
	PDP11Peripheral (bus),
	baseAddress_ {dlv11Config->baseAddress},
	baseVector_ {dlv11Config->vector},
	dlv11Config_ {dlv11Config},
	ch3BreakResponse_ {dlv11Config->ch3BreakResponse},
	breakKey_ {dlv11Config->breakKey}
{
	initialize (dlv11Config->ch3ConsoleEnabled);

	reset ();
}

// The class has a meaningful destructor so the "rule of five" applies
// and copy and move constructors and copy and move assignment operators
// have to be defined.
DLV11J::~DLV11J ()
{

}

// Initialize the DLV11J and its channels
// 
// If channel 3 is to be used as console the channel's base address
// and vector have to be set to the appropriate values.
//
// When channel 3 is configured as the console device interface [...]
// the interrupt vectors of the channel become 60 and 64. This is true
// regardless of the configured base vector of the module.
// (EK-DLV1J-UG-001)
//
void DLV11J::initialize (bool ch3ConsoleEnabled)
{
	for (u16 channelNr = 0; channelNr < numChannels; ++channelNr)
	{
		if (channelNr == 3 && ch3ConsoleEnabled)
		{
			channel_[channelNr] = make_unique<DLV11Channel> (bus_,
				defaultCh3Address_, defaultCh3Vector_, dlv11Config_);
		}
		else
		{
			channel_[channelNr] = make_unique<DLV11Channel> (bus_,
				baseAddress_ + 8 * channelNr, baseVector_ + 8 * channelNr, dlv11Config_);
		}
	}

#if 0
	if (ch3ConsoleEnabled)
		channel_[3]->send = 
			std::bind (&DLV11J::console_print, this, std::placeholders::_1);

	// Pass the console the function we want to receive the characters on
	console_->setReceiver (bind (&DLV11J::receive, this, _1, _2));
#endif // 0

	bus_->BINIT().subscribe (bind (&DLV11J::BINITReceiver, this, _1));
}

#if 0
void DLV11J::readChannel (int channelNr)
{
	unique_ptr<DLV11Channel>& ch = channel_[channelNr];
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
	unique_ptr<DLV11Channel>& ch = channel_[channelNr];
	trace.dlv11 (DLV11RecordType::DLV11_TX, channelNr, ch->xbuf);

	if (ch->send)
		ch->send ((unsigned char) ch->xbuf);
	
	ch->xcsr |= XCSR_TRANSMIT_READY;
	if (ch->xcsr & XCSR_TRANSMIT_INT)
		bus_->setInterrupt (TrapPriority::BR4, 6, ch->vector + 4);
}
#endif // 0

// This function allows the host system to read a word from one of the
// DLV11-J's registers.
StatusCode DLV11J::read (u16 address, u16 *destAddress)
{
	return channel_[3]->read (address, destAddress);
}

#if 0
void DLV11J::writeRCSR (int n, u16 value)
{
	unique_ptr<DLV11Channel>& ch = channel_[n];
	u16 old = ch->rcsr;
	ch->rcsr = (ch->rcsr & ~RCSR_WR_MASK) | (value & RCSR_WR_MASK);
	
	if ((value & RCSR_RCVR_INT) && !(old & RCSR_RCVR_INT)
			&& (ch->rcsr & RCSR_RCVR_DONE))
		bus_->setInterrupt (TrapPriority::BR4, 6, ch->vector);
}

void DLV11J::writeXCSR (int n, u16 value)
{
	unique_ptr<DLV11Channel>& ch = channel_[n];
	u16 old = ch->xcsr;
	ch->xcsr = (ch->xcsr & ~XCSR_WR_MASK) | (value & XCSR_WR_MASK);
	
	if ((value & XCSR_TRANSMIT_INT) && !(old & XCSR_TRANSMIT_INT)
			&& (ch->xcsr & XCSR_TRANSMIT_READY))
		bus_->setInterrupt (TrapPriority::BR4, 6, ch->vector + 4);
}
#endif // 0

// This function allows the host system to write a word to one of the
// DLV11-J's registers.
StatusCode DLV11J::writeWord (u16 address, u16 value)
{
	return channel_[3]->writeWord (address, value);
}


bool DLV11J::responsible (u16 address)
{
	if (address >= baseAddress_ && address <= baseAddress_ + (3 * 8))
		return true;

	/* console device */
	if (address >= 0177560 && address <= 0177566)
		return true;

	return false;
}

// On assertion of the BINIT signal initialize the device.
void DLV11J::BINITReceiver (bool signalValue)
{
	if (signalValue)
		reset ();
}

void DLV11J::reset ()
{
	for (u8 channelNr = 0; channelNr < numChannels; ++channelNr)
		channel_[channelNr]->reset ();
}

#if 0
// Put the given character in the buffer for the given channel. The buffer
// is a queue implemented as a fixed-size circular array. The array contains
// buf_size characters with the head at position buf_w.
bool DLV11J::queueCharacter (unique_ptr<DLV11Channel>& channel, unsigned char c)
{
	if (channel->buf_size < DLV11J_BUF)
	{
		channel->buf[channel->buf_w++] = c;
		channel->buf_w %= DLV11J_BUF;
		channel->buf_size++;
		return true;
	}

	return false;
}

void DLV11J::receiveDone (unique_ptr<DLV11Channel>& channel)
{
	channel->rcsr |= RCSR_RCVR_DONE;
	if (channel->rcsr & RCSR_RCVR_INT)
		bus_->setInterrupt (TrapPriority::BR4, 6, channel->vector);
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
void DLV11J::processBreak ()
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

// Receive a character from outside the system in the DLV11-J. If the
// system is powered on the received character is processed, else it is
// ignored.
void DLV11J::receive (int channelNr, unsigned char c)
{
	if (bus_->BPOK ())
	{
		if (channelNr == 3 && c == breakKey_)
		{
			// Process the BREAK, not queueing it as a received character
			processBreak ();
			return;
		}

		unique_ptr<DLV11Channel>& ch = channel_[channelNr];
		if (queueCharacter (ch, c))
		{
			trace.dlv11 (DLV11RecordType::DLV11_RX, channelNr, c);
			receiveDone (ch);
		}
	}
}


// Print a character from the DLV11-J to the outside world (i.e. the
// console in this case).
void DLV11J::console_print (unsigned char c)
{
	console_->print (c);
}
#endif // 0
