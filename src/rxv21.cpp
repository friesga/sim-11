#include <stdlib.h>
#include <string.h>

#include "lsi11.h"
#include "trace.h"

/* RX2CS bits */
#define	RX_GO			_BV(0)
#define	RX_FUNCTION_MASK	(_BV(1) | _BV(2) | _BV(3))
#define	RX_UNIT_SEL		_BV(4)
#define	RX_DONE			_BV(5)
#define	RX_INTR_ENB		_BV(6)
#define	RX_TR			_BV(7)
#define	RX_DEN			_BV(8)
#define	RX_RX02			_BV(11)
#define	RX_EXT_ADDR_MASK	(_BV(12) | _BV(13))
#define	RX_INIT			_BV(14)
#define	RX_ERROR		_BV(15)

#define	RX_RMASK		(RX_UNIT_SEL | RX_DONE | RX_INTR_ENB \
		| RX_TR | RX_DEN | RX_RX02 | RX_ERROR)
#define	RX_WMASK		(RX_GO | RX_FUNCTION_MASK | RX_UNIT_SEL \
		| RX_INTR_ENB | RX_DEN | RX_EXT_ADDR_MASK | RX_INIT)

/* RX2CS function codes */
#define	RX_FILL_BUFFER		(00 << 1)
#define	RX_EMPTY_BUFFER		(01 << 1)
#define	RX_WRITE_SECTOR		(02 << 1)
#define	RX_READ_SECTOR		(03 << 1)
#define	RX_SET_MEDIA_DENSITY	(04 << 1)
#define	RX_READ_STATUS		(05 << 1)
#define	RX_WRITE_DELETED_DATA	(06 << 1)
#define	RX_READ_ERROR_CODE	(07 << 1)

/* RX2ES bits */
#define	RX2ES_CRC		_BV(0)
#define	RX2ES_ID		_BV(2)
#define	RX2ES_RX_AC_LO		_BV(3)
#define	RX2ES_DEN_ERR		_BV(4)
#define	RX2ES_DRV_DEN		_BV(5)
#define	RX2ES_DD		_BV(6)
#define	RX2ES_DRV_RDY		_BV(7)
#define	RX2ES_UNIT_SEL		_BV(8)
#define	RX2ES_WC_OVFL		_BV(10)
#define	RX2ES_NXM		_BV(11)

#define	RX2ES_DEFAULT		(RX2ES_ID | RX2ES_DRV_DEN | RX2ES_DRV_RDY)
#define	RX2ES_ERRORS		(RX2ES_CRC | RX2ES_RX_AC_LO | RX2ES_DEN_ERR \
		| RX2ES_WC_OVFL | RX2ES_NXM)

#define	READ(addr)			(bus->Read((addr)))
#define	WRITE(addr, val)	(bus->Write((addr), (val)))
#define	CHECK()			{ if(bus->trap) return; }

void RXV21Reset ();

RXV21::RXV21 ()
{
	/* factory configuration */
	base = 0177170;
	vector = 0264;

	Reset ();
}

RXV21::~RXV21 ()
{
	/* nothing */
}

void RXV21::ClearErrors()
{
	rx2cs &= ~RX_ERROR;
	rx2es &= ~RX2ES_ERRORS;
}

void RXV21::Done ()
{
	QBUS* bus = this->bus;

	state = 0;
	rx2cs |= RX_DONE;
	rx2es |= RX2ES_DRV_DEN;
	rx2db = rx2es;

	if (rx2cs & RX_INTR_ENB) 
	{
		bus->Interrupt (vector);
	}
}

extern int trace;
void RXV21::FillBuffer ()
{
	QBUS* bus = this->bus;
	u16 limit = (rx2cs & RX_DEN) ? 128 : 64;
	u16 wc;
	u16 ptr;

	TRCRXV21DMA (TRC_RXV21_FILL, rx2wc, rx2ba);

	if (rx2wc > limit) 
	{
		TRCRXV21Error (TRC_RXV21_WC_OVFL, rx2wc);
		error = 0230; /* Word count overflow */
		rx2es |= RX2ES_WC_OVFL;
		rx2cs |= RX_ERROR;
		Done ();
		return;
	}

	memset(buffer, 0, 256);

	for (wc = rx2wc, ptr = 0; wc > 0; wc--, ptr++) 
	{
		/* Transfer words */
		buffer[ptr] = READ (rx2ba);
		rx2ba += 2;
	}

	Done ();
}

void RXV21::EmptyBuffer ()
{
	QBUS* bus = this->bus;
	u16 limit = (rx2cs & RX_DEN) ? 128 : 64;
	u16 wc;
	u16 ptr;

	TRCRXV21DMA (TRC_RXV21_EMPTY, rx2wc, rx2ba);
	if (rx2wc > limit) 
	{
		TRCRXV21Error (TRC_RXV21_WC_OVFL, rx2wc);
		error = 0230; /* Word count overflow */
		rx2es |= RX2ES_WC_OVFL;
		rx2cs |= RX_ERROR;
		Done ();
		return;
	}

	for (wc = rx2wc, ptr = 0; wc > 0; wc--, ptr++) 
	{
		/* Transfer words */
		WRITE (rx2ba, buffer[ptr]);
		rx2ba += 2;
	}
	Done ();
}

void RXV21::WriteSector ()
{
	u32 offset;

	rx2sa &= 0037;
	rx2ta &= 0177;

	offset = (rx2sa - 1) * 256 + rx2ta * (26 * 256);

	TRCRXV21CMDCommit ((rx2cs & RX_FUNCTION_MASK) >> 1, rx2cs);
	TRCRXV21Disk (TRC_RXV21_WRITE, (rx2cs & RX_UNIT_SEL) ? 1 : 0,
			(rx2cs & RX_DEN) ? 1 : 0, rx2sa, rx2ta);

	if (!(rx2cs & RX_DEN)) 
	{
		TRCRXV21Error (TRC_RXV21_DEN_ERR, 0);
		error = 0240; /* Density Error */
		rx2cs |= RX_ERROR;
		rx2es |= RX2ES_DEN_ERR;
		Done ();
		return;
	}

	if (rx2sa == 0 || rx2sa > 26 || rx2ta > 76) 
	{
		if (rx2ta > 76) 
		{
			error = 0040; /* Tried to access a track greater than 76 */
			TRCRXV21Error (TRC_RXV21_TRACK_NO, rx2ta);
		} 
		else 
		{
			error = 0070; /* Desired sector could not be found after looking at 52 headers (2 revolutions) */
			TRCRXV21Error (TRC_RXV21_SECT_NO, rx2sa);
		}
		rx2cs |= RX_ERROR;
	} 
	else 
	{
		u16 i;
		u16* sec = (u16*) &data[offset];
		memcpy(&data[offset], buffer, 256);
		for(i = 0; i < 128; i++) 
		{
			sec[i] = U16L(sec[i]);
		}
	}

	Done ();
}

void RXV21::ReadSector ()
{
	u32 offset;

	rx2sa &= 0037;
	rx2ta &= 0177;

	offset = (rx2sa - 1) * 256 + rx2ta * (26 * 256);

	TRCRXV21CMDCommit ((rx2cs & RX_FUNCTION_MASK) >> 1, rx2cs);
	TRCRXV21Disk (TRC_RXV21_READ, (rx2cs & RX_UNIT_SEL) ? 1 : 0,
			(rx2cs & RX_DEN) ? 1 : 0, rx2sa, rx2ta);

	if (!(rx2cs & RX_DEN)) 
	{
		TRCRXV21Error (TRC_RXV21_DEN_ERR, 0);
		error = 0240; /* Density Error */
		rx2cs |= RX_ERROR;
		rx2es |= RX2ES_DEN_ERR;
		Done ();
		return;
	}

	if (rx2sa == 0 || rx2sa > 26 || rx2ta > 76) 
	{
		if (rx2ta > 76) 
		{
			error = 0040; /* Tried to access a track greater than 76 */
			TRCRXV21Error (TRC_RXV21_TRACK_NO, rx2ta);
		} 
		else 
		{
			error = 0070; /* Desired sector could not be found after looking at 52 headers (2 revolutions) */
			TRCRXV21Error (TRC_RXV21_SECT_NO, rx2ta);
		}
		rx2cs |= RX_ERROR;
	} 
	else 
	{
		u16 i;
		memcpy (buffer, &data[offset], 256);
		for(i = 0; i < 128; i++) 
		{
			buffer[i] = U16L(buffer[i]);
		}
	}

	Done ();
}

void RXV21::ReadStatus ()
{
	TRCRXV21CMDCommit ((rx2cs & RX_FUNCTION_MASK) >> 1, rx2cs);

	rx2es |= RX2ES_DRV_RDY | RX2ES_DRV_DEN;
	Done ();
}

void RXV21::ReadErrorCode ()
{
	QBUS* bus = this->bus;

	TRCRXV21CMDCommit ((rx2cs & RX_FUNCTION_MASK) >> 1, rx2cs);

	/* < 7:0> Definitive Error Codes */
	/* <15:8> Word Count Register */
	WRITE (rx2ba, error | (rx2wc << 8));

	/* < 7:0> Current Track Address of Drive 0 */
	/* <15:8> Current Track Address of Drive 1 */
	WRITE (rx2ba + 2, rx2ta | (rx2ta << 8));

	/* < 7:0> Target Track of Current Disk Access */
	/* <15:8> Target Sector of Current Disk Access */
	WRITE (rx2ba + 4, rx2ta | (rx2sa << 8));

	/* <7> Unit Select Bit */
	/* <5> Head Load Bit */
	/* <6> <4> Drive Density Bit of Both Drives */
	/* <0> Density of Read Error Register Command */
	/* <15:8> Track Address of Selected Drive */
	WRITE (rx2ba + 6, _BV(5) | _BV(6) | _BV(4) | _BV(0) | (rx2ta << 8));

	Done ();
}

void RXV21::ExecuteCommand ()
{
	TRCRXV21CMD ((rx2cs & RX_FUNCTION_MASK) >> 1, rx2cs);
	rx2cs &= ~RX_GO;
	state = 1;
	switch (rx2cs & RX_FUNCTION_MASK) 
	{
		case RX_FILL_BUFFER:
			ClearErrors ();
			rx2cs &= ~RX_DONE;
			rx2cs |= RX_TR;
			break;

		case RX_EMPTY_BUFFER:
			ClearErrors ();
			rx2cs &= ~RX_DONE;
			rx2cs |= RX_TR;
			break;

		case RX_WRITE_SECTOR:
			ClearErrors ();
			rx2cs &= ~RX_DONE;
			rx2cs |= RX_TR;
			rx2es = RX2ES_DRV_RDY | RX2ES_DRV_DEN;
			break;

		case RX_READ_SECTOR:
			ClearErrors ();
			rx2cs &= ~RX_DONE;
			rx2cs |= RX_TR;
			rx2es = RX2ES_DRV_RDY | RX2ES_DRV_DEN;
			break;

		case RX_SET_MEDIA_DENSITY:
			ClearErrors ();
			printf("[RXV21] NOT IMPLEMENTED: Set Media Density\n");
			fflush(stdout);
			exit(1);
			break;

		case RX_READ_STATUS:
			ReadStatus ();
			break;

		case RX_WRITE_DELETED_DATA:
			ClearErrors ();
			printf("[RXV21] NOT IMPLEMENTED: Write Deleted Data\n");
			fflush(stdout);
			exit(1);
			break;

		case RX_READ_ERROR_CODE:
			rx2cs &= ~RX_DONE;
			rx2cs |= RX_TR;
			rx2es = RX2ES_DRV_RDY | RX2ES_DRV_DEN;
			break;
	}
}

void RXV21::Process ()
{
	TRCRXV21Step ((rx2cs & RX_FUNCTION_MASK) >> 1, state, rx2db);

	if (state == 0) 
	{
		return;
	}

	switch (rx2cs & RX_FUNCTION_MASK) 
	{
		case RX_FILL_BUFFER:
			switch(state) 
			{
				case 1: /* read RX2WC */
					rx2wc = rx2db;
					state++;
					break;

				case 2: /* read RX2BA */
					rx2ba = rx2db;
					rx2cs &= ~RX_TR;
					FillBuffer ();
					break;
			}
			break;

		case RX_EMPTY_BUFFER:
			switch (state) 
			{
				case 1: /* read RX2WC */
					rx2wc = rx2db;
					state++;
					break;

				case 2: /* read RX2BA */
					rx2ba = rx2db;
					rx2cs &= ~RX_TR;
					EmptyBuffer ();
					break;
			}
			break;

		case RX_WRITE_SECTOR:
			switch (state) 
			{
				case 1: /* read RX2SA */
					rx2sa = rx2db;
					state++;
					break;

				case 2: /* read RX2TA */
					rx2ta = rx2db;
					rx2cs &= ~RX_TR;
					WriteSector ();
					break;
			}
			break;

		case RX_READ_SECTOR:
			switch(state) 
			{
				case 1: /* read RX2SA */
					rx2sa = rx2db;
					state++;
					break;

				case 2: /* read RX2TA */
					rx2ta = rx2db;
					rx2cs &= ~RX_TR;
					ReadSector ();
					break;
			}
			break;

		case RX_READ_ERROR_CODE:
			rx2ba = rx2db;
			rx2cs &= ~RX_TR;
			ReadErrorCode ();
			break;
	}
}

u16 RXV21::Read (u16 address)
{
	if(address == base) 
	{ 
		/* RX2CS */
		return rx2cs & RX_RMASK;
	} 
	else if(address == base + 2) 
	{ 
		/* RX2DB */
		return rx2db;
	}

	return 0;
}

void RXV21::Write (u16 address, u16 value)
{
	if (address == base) 
	{ 
		/* RX2CS */
		int intr = rx2cs & RX_INTR_ENB;
		rx2cs = (rx2cs & ~RX_WMASK) | (value & RX_WMASK);
		rx2cs &= ~(RX_TR | RX_INIT | RX_ERROR);
		error = 0;
		if (value & RX_INIT) 
		{
			Reset ();
			return;
		}
		if (rx2cs & RX_GO) 
		{
			/* initiate command */
			ExecuteCommand ();
		}
		if (!intr && (value & RX_INTR_ENB) && (rx2cs & RX_DONE)) 
		{
			QBUS* bus = this->bus;
			bus->Interrupt (vector);
		}
	} else if (address == base + 2) 
	{ 
		/* RX2DB */
		rx2db = value;
		Process ();
	}
}

u8 RXV21::Responsible (u16 address)
{
	return ((address >= base) && (address <= (base + 2)));
}

void RXV21::Reset ()
{
	state = 0;
	rx2cs = RX_RX02 | RX_DONE;
	rx2es = RX2ES_DEFAULT;
	rx2db = rx2es;
}

void RXV21::SetData (u8* data)
{
	this->data = data;
}
