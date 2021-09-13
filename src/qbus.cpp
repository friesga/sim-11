#include <stdlib.h>
#include <string.h>

#include "lsi11.h"
#include "trace.h"

#define	IRCJITTER()	(rand() % QBUS_DELAY_JITTER)

QBUS::QBUS ()
{
	memset (slots, 0, sizeof (slots));
}

u16	QBUS::Read (u16 address)
{
	u8 i;
	u16 addr = address;

	address &= 0xFFFE;

	for (i = 0; i < LSI11_SIZE; i++)
	{
		QBUSMod* module = slots[i];
		if (!module)
			continue;

		if (module->Responsible (address))
		{
			u16 value = module->Read (address);
			TRCBus (TRC_BUS_RD, addr, value);
			return value;
		}
	}

	TRCBus (TRC_BUS_RDFAIL, addr, 0);
	Interrupt (004);
	return 0;
}

void QBUS::Write (u16 address, u16 value)
{
	u8 i;
	
	address &= 0xFFFE;

	for (i = 0; i < LSI11_SIZE; i++)
	{
		QBUSMod* module = slots[i];
		if (!module)
			continue;

		if (module->Responsible (address))
		{
			TRCBus (TRC_BUS_WR, address, value);
			module->Write (address, value);
			return;
		}
	}

	TRCBus (TRC_BUS_WRFAIL, address, value);
	Interrupt (004);
}

int QBUS::Interrupt (int n)
{
	if (n != 004)
	{
		if (trap)
		{
			TRCIRQ (n, TRC_IRQ_FAIL);
			return 0;
		}
		else
		{
			TRCIRQ (n, TRC_IRQ_OK);
			irq = n;
			delay = IRCJITTER ();
			return 1;
		}
	}
	else
	{
		TRCIRQ (n, TRC_IRQ_OK);
		trap = n;
		return 1;
	}
}

void QBUS::Reset ()
{
	u8 i;

	TRCBus (TRC_BUS_RESET, 0, 0);

	/* clear pending interrupts */
	trap = 0;
	irq = 0;
	delay = 0;

	for (i = 0; i < LSI11_SIZE; i++)
	{
		QBUSMod* module = slots[i];
		if (!module)
			continue;

		module->Reset ();
	}
}

void QBUS::Step ()
{
	if (delay >= QBUS_DELAY)
	{
		/* wait until last trap was serviced */
		if (!trap)
		{
			TRCIRQ (irq, TRC_IRQ_SIG);
			trap = irq;
			irq = 0;
			delay = 0;
		}
	}
	else if (irq)
		delay++;
}

void QBUS::InstallModule (int slot, QBUSMod* module)
{
	slots[slot] = module;
	module->bus = this;
}
