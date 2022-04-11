#include <stdlib.h>
#include <string.h>

#include "trace.h"
#include "qbus.h"
#include "../interruptrequest/interruptrequest.h"

#define	IRCJITTER()	(rand() % QBUS_DELAY_JITTER)

QBUS::QBUS ()
{
	memset (slots, 0, sizeof (slots));
}

u16	QBUS::read (u16 address)
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
			u16 value = module->read (address);
			TRCBus (TRC_BUS_RD, addr, value);
			return value;
		}
	}

	TRCBus (TRC_BUS_RDFAIL, addr, 0);
	interrupt (busError);
	return 0;
}

void QBUS::write (u16 address, u16 value)
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
			TRCBus (TRC_BUS_WR, address, value);
			module->write (address, value);
			return;
		}
	}

	TRCBus (TRC_BUS_WRFAIL, address, value);
	interrupt (busError);
}

// (Try to) request an interrupt. Trap 004 interrupts always succeed,
// other interrupts requests only succeed if no other traps or interrupts
// are being processed.
int QBUS::interrupt (InterruptRequest intrptReq)
{
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
}

void QBUS::reset ()
{
	u8 i;

	TRCBus (TRC_BUS_RESET, 0, 0);

	/* clear pending interrupts */
	trap = emptyIntrptReq;
	irq = emptyIntrptReq;
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
	if (delay >= QBUS_DELAY)
	{
		// Check that processing of the previous trap has finished
		if (trap.vector() == 0)
		{
			TRCIRQ (irq.vector(), TRC_IRQ_SIG);
			trap = irq;
			irq = emptyIntrptReq;
			delay = 0;
		}
	}
	else if (irq.vector() != 0)
		delay++;
}

void QBUS::installModule (int slot, QBUSModule* module)
{
	slots[slot] = module;
	module->bus = this;
}
