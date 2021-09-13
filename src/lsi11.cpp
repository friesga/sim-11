#include <stdlib.h>
#include <string.h>

#include "lsi11.h"
#include "trace.h"

u16 LSI11::Read (void* user, u16 address)
{
	u8 i;
	u16 addr = address;
	LSI11* lsi = (LSI11*) user;

	address &= 0xFFFE;

	for(i = 0; i < LSI11_SIZE; i++)
	{
		QBUSMod* module = lsi->backplane[i];
		if (!module)
		{
			continue;
		}
		if (module->Responsible (address)) 
		{
			u16 value = module->Read (address);
			TRCBus (TRC_BUS_RD, addr, value);
			return value;
		}
	}

	TRCBus (TRC_BUS_RDFAIL, addr, 0);
	lsi->bus.Interrupt (004);
	return 0;
}

void LSI11::Write (void* user, u16 address, u16 value)
{
	u8 i;
	LSI11* lsi = (LSI11*) user;

	address &= 0xFFFE;

	for (i = 0; i < LSI11_SIZE; i++) 
	{
		QBUSMod* module = lsi->backplane[i];
		if(!module)
		{
			continue;
		}
		if (module->Responsible (address)) 
		{
			TRCBus(TRC_BUS_WR, address, value);
			module->Write (address, value);
			return;
		}
	}

	TRCBus (TRC_BUS_WRFAIL, address, value);
	lsi->bus.Interrupt (004);
}

#define	IRCJITTER()	(rand() % QBUS_DELAY_JITTER)

int QBUS::Interrupt (int n)
{
	if (n != 004) 
	{
		if (trap) 
		{
			TRCIRQ(n, TRC_IRQ_FAIL);
			return 0;
		} 
		else 
		{
			TRCIRQ (n, TRC_IRQ_OK);
			irq = n;
			delay = IRCJITTER();
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
	LSI11* lsi = (LSI11*) user;

	TRCBus(TRC_BUS_RESET, 0, 0);

	/* clear pending interrupts */
	trap = 0;
	irq = 0;
	delay = 0;

	for(i = 0; i < LSI11_SIZE; i++) 
	{
		QBUSMod* module = lsi->backplane[i];
		if (!module) 
		{
			continue;
		}
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
	{
		delay++;
	}
}

LSI11::LSI11 ()
{
	KD11Init (&cpu);
	bus.trap = 0;
	bus.user = (void*) this;

	memset (backplane, 0, sizeof(backplane));
}

LSI11::~LSI11 ()
{
	/* nothing */
}

void LSI11::InstallModule (int slot, QBUSMod* module)
{
	backplane[slot] = module;
	module->bus = &bus;
}

void LSI11::Reset ()
{
	KD11Reset(&cpu);
	bus.Reset();
}

void LSI11::Step ()
{
	bus.Step ();
	KD11Step (&cpu, &bus);
}

u16	QBUS::Read (void* user, u16 addr)
{
	return ((LSI11 *) user)->Read (user, addr);
}

void QBUS::Write (void* user, u16 addr, u16 value)
{
	((LSI11*)user)->Write (user, addr, value);
}
