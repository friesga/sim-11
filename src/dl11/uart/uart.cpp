#include "uart.h"
#include "trace/trace.h"
#include "console/operatorconsole/operatorconsolefactory.h"
#include "chrono/alarmclock/alarmclock.h"

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
UART::UART (Bus* bus, UARTTypeConfig const & uartTypeConfig,
	UARTConfig& uartConfig, u16 channelNr, ConsoleConfig consoleConfig)
	:
	baseAddress {uartConfig.baseAddress_},
	vector {uartConfig.baseVector_},
	enabled_ {uartConfig.enabled_},
	bus_ {bus},
	maintenanceModeSupported_ {uartTypeConfig.maintenanceModeSupported},
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

	// If Maintenance Mode is supported the XCSR MAINT bit is writable too
	if (maintenanceModeSupported_)
		XCSR_WR_MASK |= XCSR_TRANSMIT_MAINT;

	// Create a thread running the transmitter
    transmitterThread_ = thread (&UART::transmitter, this);
	
	reset ();
}

// Destructor
UART::~UART ()
{
	// Signal the transmitter to exit
	{
		lock_guard<mutex> lock {registerAccessMutex_};
		channelRunning_ = false;
		transmitter_.notify_one ();
	}

	// And then wait for its exit
	transmitterThread_.join ();
}

void UART::reset ()
{
	lock_guard<mutex> lock {registerAccessMutex_};
	receiveBuffer_.reset ();
	rcsr &= ~(RCSR_RCVR_IE | RCSR_RCVR_DONE | RCSR_READER_ENABLE);
	rbuf &= ~RBUF_ERROR_MASK;
	xcsr = XCSR_TRANSMIT_READY;
	loopback_ = false;

	bus_->clearInterrupt (TrapPriority::BR4, 6, 
		interruptPriority (Function::Receive, channelNr_));
	bus_->clearInterrupt (TrapPriority::BR4, 6, 
			interruptPriority (Function::Transmit, channelNr_));

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
CondData<u16> UART::read (BusAddress busAddress)
{
	lock_guard<mutex> lock {registerAccessMutex_};

	switch (busAddress.registerAddress () & 06)
	{
		case RCSR:
			return {rcsr, StatusCode::Success};
			break;

		case RBUF:
			return readRBUF ();
			break;

		case XCSR:
			return {xcsr, StatusCode::Success};
			break;

		case XBUF:
			return {xbuf, StatusCode::Success};
			break;

		default:
			return {StatusCode::NonExistingMemory};
	}
	return StatusCode::Success;
}

CondData<u16> UART::readRBUF ()
{
	if (!receiveBuffer_.empty ())
	{
		rbuf = (rbuf & 0177400) | receiveBuffer_.get ();
		rcsr &= ~RCSR_RCVR_DONE;
		trace.dlv11 (DLV11RecordType::DLV11_RX, channelNr_, rbuf);
	} 

	trace.dlv11 (DLV11RecordType::DLV11_CL_RXI, channelNr_, rcsr);
	bus_->clearInterrupt (TrapPriority::BR4, 6, 
		interruptPriority (Function::Receive, channelNr_));

	return {rbuf, StatusCode::Success};
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
			writeXBUF (value);
			break;
	}

	return StatusCode::Success;
}

// Reader Enable - Write only bit - Setting this bit [...] clears receiver
// done (bit 7). (EK-DLV1J-UG-001)
// This functionality is tested by VDLAB0 test 7.
void UART::writeRCSR (u16 value)
{
	u16 old = rcsr;
	rcsr = (rcsr & ~RCSR_WR_MASK) | (value & RCSR_WR_MASK);
	
	setRecvInterruptIfEnabled (old, value);
	clearRecvInterruptIfDisabled (old, value);

	if (value & RCSR_READER_ENABLE)
	{
		receiveBuffer_.reset ();
		rcsr &= ~RCSR_RCVR_DONE;
		trace.dlv11 (DLV11RecordType::DLV11_READER_ENABLE, channelNr_, value);
	}
}

void UART::setRecvInterruptIfEnabled (u16 oldCSRvalue, u16 newCSRvalue)
{
	if ((newCSRvalue & RCSR_RCVR_IE) && !(oldCSRvalue & RCSR_RCVR_IE)
			&& (rcsr & RCSR_RCVR_DONE))
	{
		trace.dlv11 (DLV11RecordType::DLV11_SET_RXI, channelNr_, newCSRvalue);
		bus_->setInterrupt (TrapPriority::BR4, 6, 
			interruptPriority (Function::Receive, channelNr_), vector);
	}
}

void UART::clearRecvInterruptIfDisabled (u16 oldCSRvalue, u16 newCSRvalue)
{
	if ((oldCSRvalue & RCSR_RCVR_IE) && !(newCSRvalue & RCSR_RCVR_IE))
	{
		trace.dlv11 (DLV11RecordType::DLV11_CL_RXI, channelNr_, newCSRvalue);
		bus_->clearInterrupt (TrapPriority::BR4, 6, 
			interruptPriority (Function::Receive, channelNr_));
	}
}

// In handling a write of the transmit CSR two situations have to be taken
// into account:
// 1. When the transmitter became ready interrupts were disabled. When
//    interrupts are enabled later on the interrupt has to be raised then,
// 2. When the transmitter became ready interrupts were enabled and an
//    interrupt was generated. When interrupts are disabled later on the
//    interrupt has to be revoked.
//
void UART::writeXCSR (u16 value)
{
	u16 old = xcsr;
	xcsr = (xcsr & ~XCSR_WR_MASK) | (value & XCSR_WR_MASK);

	if (maintenanceModeSupported_)
	{
		if (xcsr & XCSR_TRANSMIT_MAINT)
			loopback_ = true;
		else
			loopback_ = false;
	}

	if (xcsr & XCSR_TRANSMIT_BREAK)
		sendChar (0);
	
	setXmitInterruptIfEnabled (old, value);
	clearXmitInterruptIfDisabled (old, value);
}

void UART::setXmitInterruptIfEnabled (u16 oldCSRvalue, u16 newCSRvalue)
{
	if ((newCSRvalue & XCSR_TRANSMIT_IE) && !(oldCSRvalue & XCSR_TRANSMIT_IE)
			&& (xcsr & XCSR_TRANSMIT_READY))
	{
		trace.dlv11 (DLV11RecordType::DLV11_SET_TXI, channelNr_, newCSRvalue);
		bus_->setInterrupt (TrapPriority::BR4, 6, 
			interruptPriority (Function::Transmit, channelNr_), vector + 4);
	}
}

void UART::clearXmitInterruptIfDisabled (u16 oldCSRvalue, u16 newCSRvalue)
{
	if ((oldCSRvalue & XCSR_TRANSMIT_IE) && !(newCSRvalue & XCSR_TRANSMIT_IE))
	{
		trace.dlv11 (DLV11RecordType::DLV11_CL_TXI, channelNr_, newCSRvalue);
		bus_->clearInterrupt (TrapPriority::BR4, 6, 
			interruptPriority (Function::Transmit, channelNr_));
	}
}

// Diagnostic JKDJB0 test 623 verifies that when the transmit buffer is loaded
// the transmit interrupt is cleared. This behaviour seems logical as the
// transmitter isn't ready anymore, but isn't documented in the DLV11-J User's
// Guide.
// 
// Bits 8-15 are not used.
void UART::writeXBUF (u16 value)
{
	trace.dlv11 (DLV11RecordType::DLV11_CL_TXI, channelNr_, value);
	bus_->clearInterrupt (TrapPriority::BR4, 6,
		interruptPriority (Function::Transmit, channelNr_));

	sendChar (value & 0377);
}

// The Transmit Break bit is set or reset under program control. When
// set, a continuous space level is transmitted. However, transmit done
// and transmit interrupt enable can still operate allowing software
// timing of the Break signal. When not set, normal character transmission
// can occur. (EK-DLV1J-UG-001)
//
// This implies that when Transmit Break is set Transmit Ready must not
// be cleared. This behaviour is required by at least JKDBD0 test 402.
//
void UART::sendChar (u16 value)
{
	if (!(xcsr & XCSR_TRANSMIT_BREAK))
	{
		xcsr &= ~XCSR_TRANSMIT_READY;
		trace.dlv11 (DLV11RecordType::DLV11_CLR_XMIT_RDY, channelNr_, value);
	}
	xbuf = value;
	charAvailable_ = true;
	transmitter_.notify_one ();
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
	AlarmClock alarmClock {};

	while (channelRunning_)
	{
		// Get a lock on the registerAccessMutex_. This lock will be released by
		// the transmitter_.wait() call and locked again on return of that call.
		unique_lock<mutex> lock {registerAccessMutex_};

		if (!charAvailable_)
			transmitter_.wait (lock);
		charAvailable_ = false;

		if (!channelRunning_)
			break;

		if (console_ && !loopback_)
			console_->print ((unsigned char) xbuf);

		xcsr |= XCSR_TRANSMIT_READY;
		trace.dlv11(DLV11RecordType::DLV11_SET_XMIT_RDY, channelNr_, xbuf);

		if (xcsr & XCSR_TRANSMIT_IE)
		{
			trace.dlv11 (DLV11RecordType::DLV11_SET_TXI, channelNr_, xbuf);
			bus_->setInterrupt (TrapPriority::BR4, 6, 
				interruptPriority (Function::Transmit, channelNr_), vector + 4);
		}

		// If loopback is enabled send the character to the receiver of this
		// channel.
		if (loopback_)
			receiveMutExcluded (Character {lowByte (xbuf), 
					(xcsr & XCSR_TRANSMIT_BREAK) == XCSR_TRANSMIT_BREAK});

		// The following sleep has to be done while the registerAccessMutex_
		// is unlocked as else the processor has no opportunity to send the
		// next character to the UART and the delay would be useless.
		lock.unlock ();

		// Simulate the delay caused by transferring the character from the
		// shift register over the serial line. VDLAB0 test 6 waits a maximum
		// of 100 milliseconds for XMIT_READY to become set again.
		// A delay of 500 microseconds is based on a baudrate of 19.2 Kbits/sec.
		alarmClock.sleepFor (std::chrono::microseconds (500));
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
	receiveMutExcluded (c);
}

// This function performs the actual receive with the registerAccessMutex_
// locked. The function is via receive() called from OperatorConsole and
// internally in transmitter() for the loopback functionality.
void UART::receiveMutExcluded (Character c)
{
	if (bus_->BPOK ())
	{
		if (console_ && c == breakKey_ && !loopback_)
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
			trace.dlv11 (DLV11RecordType::DLV11_TX, channelNr_, c);
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
	if (rcsr & RCSR_RCVR_IE)
	{
		trace.dlv11 (DLV11RecordType::DLV11_SET_RXI, channelNr_, 0);
		bus_->setInterrupt (TrapPriority::BR4, 6, 
			interruptPriority (Function::Receive, channelNr_), vector);
	}
}

void UART::clearReceiveError ()
{
	if ((rbuf & RBUF_ERROR) && receiveBuffer_.empty ())
	 rbuf &= ~RBUF_ERROR_MASK;
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