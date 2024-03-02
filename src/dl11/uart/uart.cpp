#include "uart.h"
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
UART::UART (Qbus* bus, UARTConfig& uartConfig,
	u16 channelNr, ConsoleConfig consoleConfig)
	:
	baseAddress {uartConfig.baseAddress_},
	vector {uartConfig.baseVector_},
	enabled_ {uartConfig.enabled_},
	bus_ {bus},
	breakResponse_ {consoleConfig.breakResponse},
	breakKey_ {consoleConfig.breakKey},
	loopback_ {uartConfig.loopback_},
	channelNr_ {channelNr},
	channelRunning_ {true},
	charAvailable_ {false}
{
	// A channel used as console will always have base address 0177560.
	if (baseAddress == 0177560)
	{
		console_ = OperatorConsoleFactory::create ();

		// Pass the console the function we want to receive the characters on
		console_->setReceiver (bind (&UART::receive, this, _1));
	}

	// Create a thread running the transmitter
    transmitterThread_ = thread (&UART::transmitter, this);
	
	reset ();
}

// Destructor
UART::~UART ()
{
	// Signal the transmitter to exit
	channelRunning_ = false;
	transmitter_.notify_one ();

	// And then wait for its exit
	transmitterThread_.join ();
}

void UART::reset ()
{
	lock_guard<mutex> lock {registerAccessMutex_};
	receiveBuffer_.reset ();
	rcsr &= ~(RCSR_RCVR_INT | RCSR_RCVR_DONE);
	rbuf &= ~RBUF_ERROR_MASK;
	xcsr = XCSR_TRANSMIT_READY;

	trace.dlv11 (DLV11RecordType::DLV11_RESET, channelNr_, rcsr);
}

bool UART::responsible (BusAddress address)
{
	return enabled_ &&
		address.registerAddress () >= baseAddress &&
		address.registerAddress () <= baseAddress + 6;
}

// This function allows the host system to read a word from one of the
// UART's registers.
StatusCode UART::read (BusAddress busAddress, u16 *destAddress)
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

void UART::readChannel ()
{
	if (!receiveBuffer_.empty ())
	{
		rbuf = (rbuf & 0177400) | receiveBuffer_.get ();
		rcsr &= ~RCSR_RCVR_DONE;
	} 
}

// This function allows the processor to write a word to one of the
// UART's registers.
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
StatusCode UART::writeWord (BusAddress busAddress, u16 value)
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
// This functionality is tested by VDLAB0 test 7.
void UART::writeRCSR (u16 value)
{
	u16 old = rcsr;
	rcsr = (rcsr & ~RCSR_WR_MASK) | (value & RCSR_WR_MASK);
	
	if ((value & RCSR_RCVR_INT) && !(old & RCSR_RCVR_INT)
			&& (rcsr & RCSR_RCVR_DONE))
	{
		trace.dlv11 (DLV11RecordType::DLV11_SEI, channelNr_, value);
		bus_->setInterrupt (TrapPriority::BR4, 6, 
			interruptPriority (Function::Receive, channelNr_), vector);
	}

	if (value & RCSR_READER_ENABLE)
	{
		receiveBuffer_.reset ();
		rcsr &= ~RCSR_RCVR_DONE;
	}
}

void UART::writeXCSR (u16 value)
{
	u16 old = xcsr;
	xcsr = (xcsr & ~XCSR_WR_MASK) | (value & XCSR_WR_MASK);
	
	if ((value & XCSR_TRANSMIT_INT) && !(old & XCSR_TRANSMIT_INT)
			&& (xcsr & XCSR_TRANSMIT_READY))
	{
		trace.dlv11 (DLV11RecordType::DLV11_SEI, channelNr_, value);
		bus_->setInterrupt (TrapPriority::BR4, 6, 
			interruptPriority (Function::Transmit, channelNr_), vector + 4);
	}
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
void UART::transmitter ()
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
		{
			trace.dlv11 (DLV11RecordType::DLV11_SEI, channelNr_, xbuf);
			bus_->setInterrupt (TrapPriority::BR4, 6, 
				interruptPriority (Function::Transmit, channelNr_), vector + 4);
		}

		// The following sleep has to be done while the registerAccessMutex_
		// is unlocked as else the processor has no opportunity to send the
		// next character to the UART and the delay would be useless.
		lock.unlock ();

		// If loopback is enabled send the character to the receiver of this
		// channel.
		if (loopback_ && !console_)
			receive (Character {lowByte (xbuf), 
					(xcsr & XCSR_TRANSMIT_BREAK) == XCSR_TRANSMIT_BREAK});

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
//
// When the receiver done bit (bit 7) is set, the computer is given one full
// character time to read the receive buffer. If the program does not read the
// data before the next character is completely assembled within the holding
// register, the UART will set the data overrun flag. The setting of the flag
// indicates the first character was lost. (EK-DLV1J-UG-001)
//
// The error indications in the Receive Buffer should be cleared by the next
// transfer when the receiver buffer has been read. This functionality is not
// documented in the DLV11-J User Guide but is described in diagnostic VDLAB0
// test 12.
//
// During normal operation, the UART checks each received character for the
// proper number of STOP bits. It does this by testing for a marking condition
// at the appropriate bit time. If the UART finds a spacing condition instead,
// it sets the framing error flag (FE). The BREAK signal is a continuous
// spacing condition, and is interpreted by the UART as a data character that
// is missing its STOP bit(s). The UART, therefore, responds to the BREAK
// signal by asserting FE H. (EK-DLV1J-UG-001)
//
// The spacing condition is simulated by the Break condition in the
// transmitted Character object.
//
void UART::receive (Character c)
{
	lock_guard<mutex> lock {registerAccessMutex_};

	if (bus_->BPOK ())
	{
		if (console_ && c == breakKey_)
		{
			// Process the BREAK, not queueing it as a received character
			processBreak ();
			return;
		}

		clearReceiveError ();

		if (c.isBreak ())
			rbuf |= RBUF_ERROR | RBUF_PARITY_ERROR | RBUF_FRAMING_ERROR;

		if (queueCharacter (c))
		{
			trace.dlv11 (DLV11RecordType::DLV11_RX, channelNr_, c);
			receiveDone ();
		}
		else
		{
			// Buffer overrun occurs
			trace.dlv11 (DLV11RecordType::DLV11_OVERRUN, channelNr_, c);
			rbuf = RBUF_OVERRUN | RBUF_ERROR;
			receiveBuffer_.reset ();
			receiveBuffer_.put (c);
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
void UART::processBreak ()
{
	if (breakResponse_ == ConsoleConfig::BreakResponse::Halt)
	{
		bus_->BHALT ().cycle ();
		return;
	}
	else if (breakResponse_ == ConsoleConfig::BreakResponse::Boot)
	{
		bus_->RESET ().cycle ();
		return;
	}
}

// Put the given character in the buffer for the given channel. The buffer
// is a queue implemented as a fixed-size circular array. The array contains
// buf_size characters with the head at position buf_w.
bool UART::queueCharacter (unsigned char c)
{
	return receiveBuffer_.put (c);
}

void UART::receiveDone ()
{
	rcsr |= RCSR_RCVR_DONE;
	if (rcsr & RCSR_RCVR_INT)
	{
		trace.dlv11 (DLV11RecordType::DLV11_SEI, channelNr_, 0);
		bus_->setInterrupt (TrapPriority::BR4, 6, 
			interruptPriority (Function::Receive, channelNr_), vector);
	}
}

void UART::clearReceiveError ()
{
	if ((rbuf & RBUF_ERROR) && receiveBuffer_.empty ())
	 rbuf &= ~RBUF_ERROR_MASK;
}

// This function sleeps until the given time point is passed. We prefer
// our own sleep_until implementation as this_thread::sleep_until seems
// to have the result that the thread isn't yielded.
void UART::sleepUntil (high_resolution_clock::time_point timePoint)
{
	while (high_resolution_clock::now() < timePoint)
        std::this_thread::yield ();
}

// This functions determines the priority of the interrupt to be generated.
// 
// Interrupt priority within the DLV11-J module is also structured. Interrupt
// priority is as follows.
//
//	Interrupt Priority	Requesting Function
//	1 (highest)			Channel 0 Receiver
//	2					Channel 1 Receiver
//	3					Channel 2 Receiver
//	4					Channel 3 Receiver
//	5					Channel 0 Transmitter
//	6					Channel 1 Transmitter
//	7					Channel 2 Transmitter
//	8 (lowest)			Channel 3 Transmitter
//
u8 UART::interruptPriority (Function function, u16 channelNr)
{
	u16 numChannels {3};

	return static_cast<u16> (function) * 10 + numChannels - channelNr;
}