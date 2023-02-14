#include "rxv21.h"
#include "../configdata/rxv21unitconfig/rxv21unitconfig.h"

#include <stdlib.h>
#include <string.h>

// Constructor for a default RXV21 device without attached files
RXV21::RXV21 (Qbus *bus)
	:
	BusDevice (bus),

	// Factory configuration
	base {0177170},
	vector {0264},
	data {nullptr}
{
	name_ = "RX";

	data = (u8*) malloc(77 * 26 * 256);
	if (!data) 
		throw "Error: cannot allocate memory for rxv21";
}

RXV21::RXV21 (Qbus *bus, shared_ptr<RXV21Config> rxConfig)
	:
	BusDevice (bus)
{
	name_ = "RX";

	// Factory configuration
	base = 0177170;
	vector = 0264;

	// Allocate memory for the floppy (RXV21) data, open the file with
	// the floppy drive data (if given) and pass the data buffer address
	// to the RXV21 controller.
	// Note that the complete contents of the file is read into the data
	// buffer.
	data = (u8*) malloc(77 * 26 * 256);
	if (!data) 
		throw "Error: cannot allocate memory for rxv21";

	// Check if unit 0 is configured and a filename is given in the 
	// configuration. If so, read the contents of the file into memory.
	if (rxConfig->rxv21UnitConfig[0] != nullptr &&
		!static_pointer_cast<RXV21UnitConfig> 
			(rxConfig->rxv21UnitConfig[0])->fileName.empty ())
	{
		FILE* floppy_file = 
			fopen (static_pointer_cast<RXV21UnitConfig> 
				(rxConfig->rxv21UnitConfig[0])->fileName.c_str(), "rb");
		if (!floppy_file) 
		{
			free(data);
			throw "Error: cannot open file " + static_pointer_cast<RXV21UnitConfig> 
				(rxConfig->rxv21UnitConfig[0])->fileName;
		}
		(void) !fread (data, 77 * 26 * 256, 1, floppy_file);
		fclose (floppy_file);
	} 
	else 
		memset (data, 0, 77 * 26 * 256);

	reset ();
}

RXV21::~RXV21 ()
{
	if (data)
		free(data);
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
	rx2cs |= RX_DONE;
	rx2es |= RX2ES_DRV_DEN;
	rx2db = rx2es;

	// Try to request an interrupt. If an interrupt or trap currently is
	// being processed, the request is held and on the next step the request
	// is tried again.
	if (rx2cs & RX_INTR_ENB) 
		bus_->setInterrupt (TrapPriority::BR4, 5, vector);
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

// Write operation on either the RX2CS or RX2DB
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
		// register write action.
		if (!intr && (value & RX_INTR_ENB) && (rx2cs & RX_DONE)) 
			bus_->setInterrupt (TrapPriority::BR4, 5, vector);
	} 
	else if (address == base + 2) 
	{ 
		/* Write RX2DB */
		rx2db = value;
		dispatch (rxv21Rx2dbFilled {});
	}

	return StatusCode::OK;
}

bool RXV21::responsible (u16 address)
{
	return address >= base && address <= (base + 2) ? true : false;
}

void RXV21::reset ()
{
	rx2cs = RX_RX02 | RX_DONE;
	rx2es = RX2ES_DEFAULT;
	rx2db = rx2es;
}

