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
			CondData<u16> value = module->read (address);
			TRCBus (TRC_BUS_RD, addr, value);
			return value;
		}
	}

	TRCBus (TRC_BUS_RDFAIL, addr, 0);
	interrupt (busError);
	return {};
}

bool QBUS::write (u16 address, u16 value)
{
	u8 i;
	
	address &= 0xFFFE;

	for (i = 0; i < LSI11_SIZE; i++)
	{
		QBUSModule* module = slots[i];
		if (!module)
			continue;

		if (module->responsible (address))
		{
			// It is presumed that all writes on addresses for which a
			// device is responsible succeed.
			TRCBus (TRC_BUS_WR, address, value);
			module->write (address, value);
			return true;
		}
	}

	TRCBus (TRC_BUS_WRFAIL, address, value);
	interrupt (busError);
	return false;
}

// (Try to) request an interrupt. Trap 004 interrupts always succeed,
// other interrupts requests only succeed if no other traps or interrupts
// are being processed.
// ToDo: Requesting an interrupt always succeeds
int QBUS::interrupt (InterruptRequest intrptReq)
{
	intrptReqQueue_.push(intrptReq);
	TRCIRQ (intrptReq.vector(), TRC_IRQ_OK);
	delay = IRCJITTER ();
	return 1;
/*
	if (intrptReq.vector() != 004)
	{
		if (trap.vector() != 0 || irq.vector() != 0)
		{
			TRCIRQ (intrptReq.vector(), TRC_IRQ_FAIL);
			return 0;
		}
		else
		{
			// No trap is being processed and no interrupt request is pending,
			// so an interrupt request can be generated. Processing of the request
			// is delayed by a random amount of steps.
			TRCIRQ (intrptReq.vector(), TRC_IRQ_OK);
			irq = intrptReq;
			delay = IRCJITTER ();
			return 1;
		}
	}
	else
	{
		TRCIRQ (intrptReq.vector(), TRC_IRQ_OK);
		trap = intrptReq;
		return 1;
	}
*/
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
