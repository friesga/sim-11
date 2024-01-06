#include "trace/trace.h"
#include "qbus.h"
#include "interruptrequest/interruptrequest.h"

#include <stdlib.h>
#include <string.h>

using std::bind;
using std::placeholders::_1;

#define	IRCJITTER()	(rand() % INTRPT_LATENCY_JITTER)

Qbus::Qbus ()
	:
	processorRunning_ {false},
	delay_ {0}
{
	ourKey_ = BINIT().subscribe (bind (&Qbus::BINITReceiver, this, _1));
}

CondData<u16> Qbus::read (BusAddress address)
{
	BusDevice *module;
	u16 addr = address;

	if ((module = responsibleModule (address)) != nullptr)
	{
		u16 value;
		if (module->read (address, &value) == StatusCode::OK)
		{
			trace.bus (BusRecordType::Read, addr, value);
			return value;
		}
		else
			return {};
	}

	return {};
}

// Set an interrupt request. The only reason this could fail is when
// a device already has set an interrupt. That would be an error on the
// part of the device and wouldn't harm.
void Qbus::setInterrupt (TrapPriority priority, 
		unsigned char busOrder, unsigned char vector)
{
	InterruptRequest intrptReq {priority, busOrder, vector};
	pushInterruptRequest (intrptReq);
}

// Push the interrupt request created by setInterrupt or setTrap to the
// interupt queue.
void Qbus::pushInterruptRequest (InterruptRequest intrptReq)
{
	intrptReqQueue_.push (intrptReq);
	trace.irq (IrqRecordType::IRQ_OK, intrptReq.vector());
	delay_ = IRCJITTER ();
}

// Clear the specified interrupt request. The InterruptRequQueue will delete
// the interrupt request equal to specified request. Equality is based on
// priority and busorder (see InterruptRequest::operator==).
void Qbus::clearInterrupt (TrapPriority priority, unsigned char busOrder)
{
	intrptReqQueue_.erase (InterruptRequest {priority, busOrder, 0});
}

// Clear all pending interrupts
void Qbus::clearInterrupts ()
{
	intrptReqQueue_.clear ();
}

// The bus itself is defined as a BINIT signal receiver too as it not
// only has to pass on the signal to all subscribed devices, but has to
// perform actions itself too.
void Qbus::BINITReceiver (bool signalValue)
{
	if (signalValue)
		reset ();
}

void Qbus::reset ()
{
	trace.bus (BusRecordType::BusReset, 0, 0);

	// Clear pending interrupts
	clearInterrupts ();

	delay_ = 0;
}

// Wait a random number (max INTRPT_LATENCY) of steps till processing of an
// interrupt request. 
void Qbus::step ()
{
	++delay_;
}

// Check if there is an interrupt request available that can be processed.
// Wait a random number (max INTRPT_LATENCY) of steps till processing of an
// interrupt request. The delay counter is incremented every Qbus step.
// Traps are handled internally in the CPU and are to be processed immediately.
//
// This delay doesn't seem to be necessary to pass the VKAAC0 (for the
// KD11-NA) and the JKDBD0 (for the KDF11-A) diagnostics. These diagnostics
// however require a realistic delay on the DLV11-J transmitter XMIT-READY
// bit. As that delay is implemented the interrupt latency seems superfluous.
// For the time being left the mechanism intact and disabled the actual delay.
//
bool Qbus::intrptReqAvailable() 
{
	// return (!intrptReqQueue_.empty() && delay_ >= INTRPT_LATENCY);
	return (!intrptReqQueue_.empty());
}

// Return the priority of the interrupt request with the highest priority
u8 Qbus::intrptPriority()
{
	return static_cast<u8> (intrptReqQueue_.top().priority());
}

// Get the interrupt request with the highest priority if one is available
bool Qbus::getIntrptReq(InterruptRequest &intrptReq)
{
	if (intrptReqAvailable())
	{
		bool result = intrptReqQueue_.fetchTop (intrptReq);
		trace.irq (IrqRecordType::IRQ_SIG, intrptReq.vector());
		delay_ = 0;
		return result;
	}
	else
		return false;
}

// Install the given device in the given slot. Every PDP11Peripheral contains a
// pointer to the bus it is installed on. This pointer has to be specified
// in the device's constructor. There is something to be said for setting
// the pointer in the installModule function but in e.g. the KD11_NA device
// the pointer is already used in the constructor.
void Qbus::installModule (int slot, BusDevice *module)
{
	if (slot < LSI11_SIZE && module != nullptr)
		slots[slot] = module;
}
