#include <string.h>
#include <stdlib.h>

#include "lsi11.h"
#include "trace.h"

#define	_A(x)		(1 << ((x) - 1))
#define	_B(x)		(1 << ((x) + 7))

#define	BDV11_CPU_TEST	_A(1)
#define	BDV11_MEM_TEST	_A(2)	
#define	BDV11_DECNET	_A(3)
#define	BDV11_DIALOG	_A(4)
#define	BDV11_LOOP	_B(1)
#define	BDV11_RK05	_A(8)
#define	BDV11_RL01	_A(7)
#define	BDV11_RX01	_A(6)
#define	BDV11_RX02	(_A(6) | _A(7))
#define	BDV11_ROM	_A(5)

#define	BDV11_EXT_DIAG	_B(2)
#define	BDV11_2780	_B(3)
#define	BDV11_PROG_ROM	_B(4)

#define	BDV11_SWITCH	(BDV11_CPU_TEST | BDV11_MEM_TEST \
		| BDV11_DIALOG | BDV11_RX02)

BDV11::BDV11 ()
{
	/* nothing */
}

BDV11::~BDV11 ()
{
	/* nothing */
}

u16 BDV11::GetWordLow (u16 word)
{
	u16 page = pcr & 0xFF;
	if (page < 0x10)
	{
		u16 romword = page * 0200 + word;
		return bdv11_e53[romword];
	}
	else
	{
		return 0177777;
	}
}

u16 BDV11::GetWordHigh (u16 word)
{
	u16 page = (pcr >> 8) & 0xFF;
	if (page < 0x10)
	{
		u16 romword = page * 0200 + word;
		return bdv11_e53[romword];
	} 
	else 
	{
		return 0233;
	}
}

void BDV11::MemoryDump (u16 pcr, int hi)
{
	const u16* data;
	u16 addr;
	u16 len = 0400;
	if (hi)
	{
		addr = 0173400;
		u16 page = (pcr >> 8) & 0xFF;
		if (page < 0x10) 
		{
			u16 romword = page * 0200;
			data = &bdv11_e53[romword];
		} 
		else 
		{
			data = NULL;
		}
	} 
	else 
	{
		addr = 0173000;
		u16 page = pcr & 0xFF;
		if (page < 0x10) 
		{
			u16 romword = page * 0200;
			data = &bdv11_e53[romword];
		} 
		else 
		{
			data = NULL;
		}
	}

	if (data) 
	{
		TRCMemoryDump ((u8*) data, addr, len);
	} 
	else 
	{
		u8 buf[0400];
		memset (buf, 0xFF, sizeof(buf));
		TRCMemoryDump ((u8*) data, addr, len);
	}
}

u16 BDV11::Read (u16 address)
{
	switch (address) 
	{
		case 0177520:
			return pcr;

		case 0177522:
			return scratch;

		case 0177524:
			return BDV11_SWITCH;

		case 0177546:
			return ltc;

		default:
			if (address >= 0173000 && address < 0173400)
			{
				return GetWordLow ((address - 0173000) / 2);
			} 
			else if (address >= 0173400 && address < 0173776) 
			{
				return GetWordHigh ((address - 0173400) / 2);
			}
			return 0;
	}
}

void BDV11::Write (u16 address, u16 value)
{
	switch (address)
	{
		case 0177520:
			/* record new memory content in trace */
			if (value != pcr)
			{
				if ((value & 0xFF) == (pcr & 0xFF)) 
				{
					MemoryDump (value, 1);
				} 
				else if ((value & 0xFF00) == (pcr & 0xFF00)) 
				{
					MemoryDump (value, 0);
				} 
				else 
				{
					MemoryDump (value, 0);
					MemoryDump (value, 1);
				}
			}
			pcr = value;
			break;

		case 0177522:
			scratch = value;
			break;

		case 0177524:
			display = value;
			break;

		case 0177546:
			ltc = value & 040;
			break;
	}
}

u8 BDV11::Responsible (u16 address)
{
	switch (address)
	{
		case 0177520:
		case 0177522:
		case 0177524:
		case 0177546:
			return 1;
		default:
			return address >= 0173000 && address <= 0173776;
	}
}

void BDV11::Reset ()
{
	pcr = 0;
	scratch = 0;
	display = 0;
	ltc = 0;

	MemoryDump (pcr, 0);
	MemoryDump (pcr, 1);
}



void BDV11::Step (float dt)
{
	if (ltc & 040) 
	{
		time += dt;
		if (time >= LTC_TIME) 
		{
			QBUS* bus = this->bus;
			bus->interrupt (bus, 0100);
			time -= LTC_TIME;
			if (time >= LTC_TIME) 
			{
				time = 0;
			}
		}
	} 
	else 
	{
		time = 0;
	}
}
