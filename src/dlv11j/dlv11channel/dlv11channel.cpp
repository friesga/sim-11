#include "dlv11channel.h"
#include "trace/trace.h"
#include "console/operatorconsole/operatorconsolefactory.h"

#include <memory>
#include <functional>
#include <chrono>
#include <thread>

using std::bind;
using std::placeholders::_1;
using std::lock_guard;
using std::unique_lock;
using std::chrono::high_resolution_clock;
using std::chrono::duration;

// Constructor
DLV11Channel::DLV11Channel (Qbus* bus, u16 channelBaseAddress, 
	u16 channelVector, shared_ptr<DLV11Config> dlv11Config)
	:
	baseAddress {channelBaseAddress},
	vector {channelVector},
	bus_ {bus},
	ch3BreakResponse_ {dlv11Config->ch3BreakResponse},
	breakKey_ {dlv11Config->breakKey},
	channelRunning_ {true},
	charAvailable_ {false}
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

	// Create a thread running the transmitter
    transmitterThread_ = thread (&DLV11Channel::transmitter, this);
	
	reset ();
}

// Destructor
DLV11Channel::~DLV11Channel ()
{
	// Signal the transmitter to exit
	channelRunning_ = false;
	transmitter_.notify_one ();

	// And then wait for its exit
	transmitterThread_.join ();
}

void DLV11Channel::reset ()
{
	lock_guard<mutex> lock {registerAccessMutex_};
	rcsr &= ~(RCSR_RCVR_INT | RCSR_RCVR_DONE);
	xcsr = XCSR_TRANSMIT_READY;
}

// This function allows the host system to read a word from one of the
// DLV11-J's registers.
StatusCode DLV11Channel::read (BusAddress busAddress, u16 *destAddress)
{
	lock_guard<mutex> lock {registerAccessMutex_};

	switch (busAddress.registerAddress () & 06)
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
	if (!rBuffer_.empty ())
	{
		rbuf = rBuffer_.get ();
		rcsr &= ~RCSR_RCVR_DONE;
	} 
	else
	{
		rbuf = RBUF_OVERRUN;
		if (rbuf & RBUF_ERROR_MASK)
			rbuf |= RBUF_ERROR;
	}
}

// This function allows the processor to write a word to one of the
// DLV11-J's registers.
// 
// Access to the registers has to be synchronized as the transmitter runs in
// a sperate thread. The registers are accessed in five functions:
// - The processor reading a register (read),
// - The processor writing a register (writeWord),
// - The transmitter transmitting a character to the operator console
//   (transmitter), 
// - The operator console receiving data (receive),
// - Reset caused by a bus initialization (reset).
// 
// Each of these functions therefore has to lock the registerAccessMutex_.
//
StatusCode DLV11Channel::writeWord (BusAddress busAddress, u16 value)
{
	lock_guard<mutex> lock {registerAccessMutex_};

	switch (busAddress.registerAddress () & 06)
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
			xcsr &= ~XCSR_TRANSMIT_READY;
			xbuf = value;
			charAvailable_ = true;
			transmitter_.notify_one ();
			break;
	}

	return StatusCode::OK;
}

// Reader Enable - Write only bit - Setting this bit [...] clears receiver
// done (bit 7). (EK-DLV1J-UG-001)
// This functionaility is tested by VDLAB0 test 7.
void DLV11Channel::writeRCSR (u16 value)
{
	u16 old = rcsr;
	rcsr = (rcsr & ~RCSR_WR_MASK) | (value & RCSR_WR_MASK);

	if (value & RCSR_READER_ENABLE)
		rcsr &= ~RCSR_RCVR_DONE;
	
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

// Transmit data to the receiver. For channel 3 the data is sent to the
// operator console, for the other channels the data currently disappears
// into a vacuum.
//
// The transmitter runs in its own thread to create a realistic timing on
// Transmitter Ready bit in the XCSR. The DLV11-J transmitter is double
// buffered. It comprises two registers, a holding register which receives
// characters from the processor and a shift register from which the characters
// are sent over the serial line. On receipt of a character in the holding
// register XMIT_READY is cleared, the character is transferred to the shift
// register, after which XMIT_READY is set again as the holding register is
// ready to accept another register. On receipt of a second character while
// the shift register is not yet empty XMIT_READY is cleared and not set
// until the shift register is empty and the character can be moved to that
// register. This implies that the first received character can be processed
// immediately but a time delay is present on receipt of a second character
// while the first character is being processed. See EK-DLV11J-UG-001
// par. 4.9.4 (Transmit Operation).
// 
// This timing behaviour is implemented to satisfy at least VDLAB0 TEST 6.
//
void DLV11Channel::transmitter ()
{
	high_resolution_clock::time_point nextCharCanBeProcessedAt = 
		high_resolution_clock::now ();

	while (channelRunning_)
	{
		// Get a lock on the registerAccessMutex_. This lock will be released by
		// the transmitter_.wait() call and lock again on return of that call.
		unique_lock<mutex> lock {registerAccessMutex_};

		if (!charAvailable_)
			transmitter_.wait (lock);
		charAvailable_ = false;

		if (!channelRunning_)
			break;

		if (console_)
			console_->print ((unsigned char) xbuf);

		xcsr |= XCSR_TRANSMIT_READY;
		if (xcsr & XCSR_TRANSMIT_INT)
			bus_->setInterrupt (TrapPriority::BR4, 6, vector + 4);

		// The following sleep has to be done while the registerAccessMutex_
		// is unlocked as else the processor has no opportunity to send the
		// next character to the DLV11-J and the delay would be useless.
		lock.unlock ();

		// Loopback char on other channels
		if (!console_)
			receive ((unsigned char) xbuf);

		// Simulate the delay caused by transferring the character from the
		// shift register over the serial line. VDLAB0 test 6 waits a maximum
		// of 100 milliseconds for XMIT_READY to become set again. A wait of
		// 1 millisecond real time satisfies the test.
		nextCharCanBeProcessedAt = high_resolution_clock::now () + 
			std::chrono::microseconds (1000);

		sleepUntil (nextCharCanBeProcessedAt);
	}
}

// The functions above interface with the processor; the following functions
// interface with the Console.
//
// Receive a character from outside the system in the DLV11-J. If the
// system is powered on the received character is processed, else it is
// ignored.
void DLV11Channel::receive (unsigned char c)
{
	lock_guard<mutex> lock {registerAccessMutex_};

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
	return rBuffer_.put (c);
}

void DLV11Channel::receiveDone ()
{
	rcsr |= RCSR_RCVR_DONE;
	if (rcsr & RCSR_RCVR_INT)
		bus_->setInterrupt (TrapPriority::BR4, 6, vector);
}

// This function sleeps until the given time point is passed. We prefer
// our own sleep_until implementation as this_thread::sleep_until seems
// to have the result that the thread isn't yielded.
void DLV11Channel::sleepUntil (high_resolution_clock::time_point timePoint)
{
	while (high_resolution_clock::now() < timePoint)
        std::this_thread::yield ();
}