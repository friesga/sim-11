#include "rxv21.h"

#include <stdlib.h>
#include <string.h>

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

	rx2cs |= RX_DONE;
	rx2es |= RX2ES_DRV_DEN;
	rx2db = rx2es;

	// Try to request an interrupt. If an interrupt or trap currently is
	// being processed, the request is held and on the next step the request
	// is tried again.
	if (rx2cs & RX_INTR_ENB) 
		IRQ(interruptRequest(vector));
}

// Read operation on either the RX2CS or RX2DB
StatusCode RXV21::read (u16 address, u16 *destAddress)
{
	if(address == base) 
		/* Return value of the RX2CS */
		*destAddress = rx2cs & RX_RMASK;
	else if(address == base + 2) 
		/* Return value of the RX2DB */
		*destAddress = rx2db;

	return StatusCode::OK;
}

// Write operation on either the RX2CS of RX2DB
// A command to the RXV21 will be executed in two or more steps:
// 1. The selection and initiation of the command to be executed by
//	  writing the RX2CS,
// 2. Writing parameters of the command to the RX2DB, such as the sector
//	  and track address or the bus address.
//
StatusCode RXV21::writeWord (u16 address, u16 value)
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
			dispatch (rxv21Init {});
			return StatusCode::OK;
		}

		if (rx2cs & RX_GO) 
			/* Initiate command given */
			dispatch (rxv21Go {});

		// Generate an interrupt for the last succesfully completed command
		// if interrupts were not enabled and Interrupt Enable is set in this
		// register write
		if (!intr && (value & RX_INTR_ENB) && (rx2cs & RX_DONE)) 
		{
			QBUS* bus = this->bus;
			IRQ(interruptRequest(vector));
		}
	} 
	else if (address == base + 2) 
	{ 
		/* Write RX2DB */
		rx2db = value;
		dispatch (rxv21Rx2dbFilled {});
	}

	return StatusCode::OK;
}

u8 RXV21::responsible (u16 address)
{
	return ((address >= base) && (address <= (base + 2)));
}

void RXV21::reset ()
{
	rx2cs = RX_RX02 | RX_DONE;
	rx2es = RX2ES_DEFAULT;
	rx2db = rx2es;
}

void RXV21::setData (u8* data)
{
	this->data = data;
}

// Originally in this function missed interrupt requests were checked
// and re-issued. This is now superfluous because of the introduction of
// the interrupt request queue.
void RXV21::step()
{}
