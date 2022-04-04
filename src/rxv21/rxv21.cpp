#include <stdlib.h>
#include <string.h>

#include "trace.h"
#include "rxv21.h"

/* RX2CS bits */
#define	RX_GO				_BV(0)
#define	RX_FUNCTION_MASK	(_BV(1) | _BV(2) | _BV(3))
#define	RX_UNIT_SEL			_BV(4)
#define	RX_DONE				_BV(5)
#define	RX_INTR_ENB			_BV(6)
#define	RX_TR				_BV(7)
#define	RX_DEN				_BV(8)
#define	RX_RX02				_BV(11)
#define	RX_EXT_ADDR_MASK	(_BV(12) | _BV(13))
#define	RX_INIT				_BV(14)
#define	RX_ERROR			_BV(15)

// RX2CS Read Mask bits
#define	RX_RMASK		(RX_UNIT_SEL | RX_DONE | RX_INTR_ENB \
		| RX_TR | RX_DEN | RX_RX02 | RX_ERROR)

// RX2CS Write Mask bits
#define	RX_WMASK		(RX_GO | RX_FUNCTION_MASK | RX_UNIT_SEL \
		| RX_INTR_ENB | RX_DEN | RX_EXT_ADDR_MASK | RX_INIT)

/* RX2CS function codes */
#define	RX_FILL_BUFFER			(00 << 1)
#define	RX_EMPTY_BUFFER			(01 << 1)
#define	RX_WRITE_SECTOR			(02 << 1)
#define	RX_READ_SECTOR			(03 << 1)
#define	RX_SET_MEDIA_DENSITY	(04 << 1)
#define	RX_READ_STATUS			(05 << 1)
#define	RX_WRITE_DELETED_DATA	(06 << 1)
#define	RX_READ_ERROR_CODE		(07 << 1)

/* RX2ES bits */
#define	RX2ES_CRC			_BV(0)
#define	RX2ES_ID			_BV(2)
#define	RX2ES_RX_AC_LO		_BV(3)
#define	RX2ES_DEN_ERR		_BV(4)
#define	RX2ES_DRV_DEN		_BV(5)
#define	RX2ES_DD			_BV(6)
#define	RX2ES_DRV_RDY		_BV(7)
#define	RX2ES_UNIT_SEL		_BV(8)
#define	RX2ES_WC_OVFL		_BV(10)
#define	RX2ES_NXM			_BV(11)

#define	RX2ES_DEFAULT		(RX2ES_ID | RX2ES_DRV_DEN | RX2ES_DRV_RDY)
#define	RX2ES_ERRORS		(RX2ES_CRC | RX2ES_RX_AC_LO | RX2ES_DEN_ERR \
		| RX2ES_WC_OVFL | RX2ES_NXM)

#define	READ(addr)			(bus->read((addr)))
#define	WRITE(addr, val)	(bus->write((addr), (val)))
#define	CHECK()				{ if (bus->trap) return; }

#define	IRQ(x)				if (!bus->interrupt(x)) irq = (x)

extern int trace;

RXV21::RXV21 ()
{
	/* factory configuration */
	base = 0177170;
	vector = 0264;

	reset ();
}

RXV21::~RXV21 ()
{
	/* nothing */
}

void RXV21::clearErrors()
{
	rx2cs &= ~RX_ERROR;
	rx2es &= ~RX2ES_ERRORS;
}

// A RXV21 command is completed. Upon completion the Done bit is set
// and the RX2DB is loaded with the RX2ES. If the host program has enabled
// interrupts an interrupt will occur.
void RXV21::done ()
{
	QBUS* bus = this->bus;

	state = 0;
	rx2cs |= RX_DONE;
	rx2es |= RX2ES_DRV_DEN;
	rx2db = rx2es;

	// Try to request an interrupt. If an interrupt or trap currently is
	// being processed, the request is held and on the next step the request
	// is tried again.
	if (rx2cs & RX_INTR_ENB) 
		IRQ(vector);
}

// This function fills the RX02 data buffer with the number of words
// of data specified by the RX2WC register from the bus address specified
// by the RX2BA register.
void RXV21::fillBuffer ()
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
		done ();
		return;
	}

	memset(buffer, 0, 256);

	for (wc = rx2wc, ptr = 0; wc > 0; wc--, ptr++) 
	{
		/* Transfer words */
		buffer[ptr] = READ (rx2ba);
		rx2ba += 2;
	}

	done ();
}

// This function empties the contents of the internal buffer for use by
// the host processor. This data is in the buffer as the result of a
// previous Fill Buffer or Read Sector command.
void RXV21::emptyBuffer ()
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
		done ();
		return;
	}

	for (wc = rx2wc, ptr = 0; wc > 0; wc--, ptr++) 
	{
		/* Transfer words */
		WRITE (rx2ba, buffer[ptr]);
		rx2ba += 2;
	}
	done ();
}

// This function locates the desired sector on the diskette and fills it
// with the contents of the internal buffer.
// 
// ToDo: Replace numbers by meaningful constants
//
void RXV21::writeSector ()
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
		done ();
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

	done ();
}

// This function is locates the desired sector and transfers the contents
// of the data field to the internal buffer in the control. This function may
// also be used to retrieve rapidly (5 ms) the current status of the drive
// selected.
void RXV21::readSector ()
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
		done ();
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

	done ();
}

// Th Maintenance Read Status command has the following effect:
// - Done is cleared. 
// - The Drive Ready bit (bit 7 RX2ES) is updated by counting index pulses in
//   the control. 
// - The Drive Density is updated by loading the head of the selected drive and
//   reading the first data mark. 
// - The RX2ES is moved into the RX2DB.
void RXV21::readStatus ()
{
	TRCRXV21CMDCommit ((rx2cs & RX_FUNCTION_MASK) >> 1, rx2cs);

	rx2es |= RX2ES_DRV_RDY | RX2ES_DRV_DEN;
	done ();
}

// The Read Error Code function implies a read extended status. In addition
// to the specific error code a dump of the control's internal scratch pad
// registers also occurs. This is the only way that the word count register
// can be retrieved. This function is used to retrieve specific information
// as well as drive status information depending upon detection of the general 
// Error bit.
void RXV21::readErrorCode ()
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

	done ();
}

// Initiate a command to the RX02.
// The Maintenance Read Status and Read Error Code commands can be performed
// immediately, the Set Media Density and Write Deleted Data commands are
// currently unsupported. The other commands require additional loading of
// parameters into the RX2DB before they can be executed.
void RXV21::initiateCommand ()
{
	TRCRXV21CMD ((rx2cs & RX_FUNCTION_MASK) >> 1, rx2cs);
	rx2cs &= ~RX_GO;
	state = 1;

	// Perform the selected function
	switch (rx2cs & RX_FUNCTION_MASK) 
	{
		case RX_FILL_BUFFER:
			clearErrors ();
			rx2cs &= ~RX_DONE;
			rx2cs |= RX_TR;
			break;

		case RX_EMPTY_BUFFER:
			clearErrors ();
			rx2cs &= ~RX_DONE;
			rx2cs |= RX_TR;
			break;

		case RX_WRITE_SECTOR:
			clearErrors ();
			rx2cs &= ~RX_DONE;
			rx2cs |= RX_TR;
			rx2es = RX2ES_DRV_RDY | RX2ES_DRV_DEN;
			break;

		case RX_READ_SECTOR:
			clearErrors ();
			rx2cs &= ~RX_DONE;
			rx2cs |= RX_TR;
			rx2es = RX2ES_DRV_RDY | RX2ES_DRV_DEN;
			break;

		case RX_SET_MEDIA_DENSITY:
			clearErrors ();
			printf("[RXV21] NOT IMPLEMENTED: Set Media Density\n");
			fflush(stdout);
			exit(1);
			break;

		case RX_READ_STATUS:
			readStatus ();
			break;

		case RX_WRITE_DELETED_DATA:
			clearErrors ();
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

// Process the RX2DB register contents, depending on the function
// selected in the RX2CS.
// 
// Note that the TR bit is not deasserted and asserted in between
// steps.
void RXV21::process ()
{
	TRCRXV21Step ((rx2cs & RX_FUNCTION_MASK) >> 1, state, rx2db);

	if (state == 0) 
		return;

	switch (rx2cs & RX_FUNCTION_MASK) 
	{
		case RX_FILL_BUFFER:
			// After function selection the Fill Buffer command requires
			// two additional steps:
			// 1. Loading of the EX2DB register with the word count (RX2WC),
			// 2. Loading of the EX2DB with the bus address (RX2BA). This will
			//	  start the command execution.
			switch (state) 
			{
				case 1: /* read RX2WC */
					rx2wc = rx2db;
					state++;
					break;

				case 2: /* read RX2BA */
					rx2ba = rx2db;
					rx2cs &= ~RX_TR;
					fillBuffer ();
					break;
			}
			break;

		case RX_EMPTY_BUFFER:
			// After function selection the Empty Buffer command requires
			// two additional steps:
			// 1. Loading of the EX2DB register with the word count (RX2WC),
			// 2. Loading of the EX2DB with the bus address (RX2BA). This will
			//	  start the command execution.
			switch (state) 
			{
				case 1: /* read RX2WC */
					rx2wc = rx2db;
					state++;
					break;

				case 2: /* read RX2BA */
					rx2ba = rx2db;
					rx2cs &= ~RX_TR;
					emptyBuffer ();
					break;
			}
			break;

		case RX_WRITE_SECTOR:
			// After function selection the Write Sector command requires
			// two additional steps:
			// 1. Loading of the EX2DB register with the desired sector address (RX2SA),
			// 2. Loading of the EX2DB with the desired track address (RX2TA). This will
			//	  start the command execution.
			switch (state) 
			{
				case 1: /* read RX2SA */
					rx2sa = rx2db;
					state++;
					break;

				case 2: /* read RX2TA */
					rx2ta = rx2db;
					rx2cs &= ~RX_TR;
					writeSector ();
					break;
			}
			break;

		case RX_READ_SECTOR:
			// After function selection the Read Sector command requires
			// two additional steps:
			// 1. Loading of the EX2DB register with the desired sector address (RX2SA),
			// 2. Loading of the EX2DB with the desired track address (RX2TA). This will
			//	  start the command execution.
			switch(state) 
			{
				case 1: /* read RX2SA */
					rx2sa = rx2db;
					state++;
					break;

				case 2: /* read RX2TA */
					rx2ta = rx2db;
					rx2cs &= ~RX_TR;
					readSector ();
					break;
			}
			break;

		case RX_READ_ERROR_CODE:
			// After function selection the Write Sector command requires
			// one additional step:
			// 1. Loading of the EX2DB register with the bus adress. This will
			//	  start the command.
			rx2ba = rx2db;
			rx2cs &= ~RX_TR;
			readErrorCode ();
			break;
	}
}

// Read operation on either the RX2CS or RX2DB
u16 RXV21::read (u16 address)
{
	if(address == base) 
		/* Return value of the RX2CS */
		return rx2cs & RX_RMASK;
	else if(address == base + 2) 
		/* Return value of the RX2DB */
		return rx2db;

	return 0;
}

// Write operation on either the RX2CS of RX2DB
// A command to the RXV21 will be executed in two or more steps:
// 1. The selection and initiation of the command to be executed by
//	  writing the RX2CS,
// 2. Writing parameters of the command to the RX2DB, such as the sector
//	  and track address or the bus address.
//
void RXV21::write (u16 address, u16 value)
{
	if (address == base) 
	{ 
		/* Write RX2CS */
		int intr = rx2cs & RX_INTR_ENB;
		rx2cs = (rx2cs & ~RX_WMASK) | (value & RX_WMASK);
		rx2cs &= ~(RX_TR | RX_INIT | RX_ERROR);
		error = 0;

		if (value & RX_INIT) 
		{
			// Initialize command given
			reset ();
			return;
		}

		if (rx2cs & RX_GO) 
			/* Initiate command given */
			initiateCommand ();

		if (!intr && (value & RX_INTR_ENB) && (rx2cs & RX_DONE)) 
		{
			QBUS* bus = this->bus;
			IRQ(vector);
		}
	} 
	else if (address == base + 2) 
	{ 
		/* Write RX2DB */
		rx2db = value;
		process ();
	}
}

u8 RXV21::responsible (u16 address)
{
	return ((address >= base) && (address <= (base + 2)));
}

void RXV21::reset ()
{
	state = 0;
	rx2cs = RX_RX02 | RX_DONE;
	rx2es = RX2ES_DEFAULT;
	rx2db = rx2es;
	irq = 0;
}

void RXV21::setData (u8* data)
{
	this->data = data;
}

void RXV21::step()
{
	// Is a pending interrupt request available?
	if (irq)
	{
		// Retry the interrupt request and if it succeeds
		// clear the pending request.
		QBUS* bus = this->bus;
		if (bus->interrupt(irq))
			irq = 0;
	}
}
