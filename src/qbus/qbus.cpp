#include <stdlib.h>
#include <string.h>

#include "trace.h"
#include "qbus.h"
#include "interruptrequest/interruptrequest.h"

#define	IRCJITTER()	(rand() % QBUS_DELAY_JITTER)

QBUS::QBUS ()
{
	memset (slots, 0, sizeof (slots));
}

CondData<u16> QBUS::read (u16 address)
{
	u8 i;
	u16 addr = address;

	address &= 0xFFFE;

	for (i = 0; i < LSI11_SIZE; i++)
	{
		QBUSModule* module = slots[i];
		if (!module)
			continue;

		if (module->responsible (address))
		{
			// CondData<u16> value = module->read (address);
			u16 value;
			module->read (address, &value);
			TRCBus (TRC_BUS_RD, addr, value);
			return value;
		}
	}

	TRCBus (TRC_BUS_RDFAIL, addr, 0);
	setTrap (TrapPriority::BusError, 004);
	return {};
}


// Set an interrupt request. The only reason this could fail is when
// a device already has set an interrupt. That would be an error on the
// part of the device and wouldn't harm.
void QBUS::setInterrupt (TrapPriority priority, 
		unsigned char busOrder, unsigned char vector)
{
	InterruptRequest intrptReq {RequestType::IntrptReq, priority, busOrder, vector};
	pushInterruptRequest (intrptReq);
}


// Set a trap
void QBUS::setTrap (TrapPriority priority, unsigned char vector)
{
	InterruptRequest trap {RequestType::Trap, priority, 0, vector};
	pushInterruptRequest (trap);
}

// Push the interrupt request created by setInterrupt or setTrap to the
// interupt queue.
void QBUS::pushInterruptRequest (InterruptRequest intrptReq)
{
	intrptReqQueue_.push (intrptReq);
	TRCIRQ (intrptReq.vector(), TRC_IRQ_OK);
	delay = IRCJITTER ();
}

void QBUS::reset ()
{
	u8 i;

	TRCBus (TRC_BUS_RESET, 0, 0);

	/* Clear pending interrupts */
	intrptReqQueue_.clear();

	delay = 0;

	for (i = 0; i < LSI11_SIZE; i++)
	{
		QBUSModule* module = slots[i];
		if (!module)
			continue;

		module->reset ();
	}
}

// Wait a random number (max QBUS_DELAY) of steps till processing of an
// interrupt request. 
void QBUS::step ()
{
	++delay;
}

// Check if there is an interrupt request available that can be processed.
// Wait a random number (max QBUS_DELAY) of steps till processing of an
// interrupt request. The delay counter is incremented every QBUS step.
// Traps are handled internally in the CPU and are to be processed immediately.
bool QBUS::intrptReqAvailable() 
{
	return (!intrptReqQueue_.empty() && 
		(delay >= QBUS_DELAY || intrptReqQueue_.top().requestType() == RequestType::Trap));
}

// Return the priority of the interrupt request with the highest priority
u8 QBUS::intrptPriority()
{
	return static_cast<u8> (intrptReqQueue_.top().priority());
}

// Get the interrupt request with the highest priority if one is available
bool QBUS::getIntrptReq(InterruptRequest &intrptReq)
{
	if (intrptReqAvailable())
	{
		bool result = intrptReqQueue_.fetchTop (intrptReq);
		TRCIRQ (intrptReq.vector(), TRC_IRQ_SIG);
		delay = 0;
		return result;
	}
	else
		return false;
}

void QBUS::installModule (int slot, QBUSModule* module)
{
	slots[slot] = module;
	module->bus = this;
}
