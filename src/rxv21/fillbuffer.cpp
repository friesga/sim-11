#include "rxv21.h"

// After function selection the Fill Buffer command requires
// two additional steps:
// 1. Loading of the EX2DB register with the word count (RX2WC),
// 2. Loading of the EX2DB with the bus address (RX2BA). This will
//	  start the command execution.
// 
// Note that the TR bit is not deasserted and asserted in between
// steps.
void RXV21::entry (rxv21FillBufferRx2wc)
{
	clearErrors();
    rx2cs &= ~RX_DONE;
    rx2cs |= RX_TR;
}

State RXV21::transition (rxv21FillBufferRx2wc &&, rxv21Rx2dbFilled)
{
    // ToDo: Provide useful state to TRCRXV21Step
    TRCRXV21Step ((rx2cs & RX_FUNCTION_MASK) >> 1, 1, rx2db);
    rx2wc = rx2db;
    return rxv21FillBufferRx2ba {};
}

State RXV21::transition (rxv21FillBufferRx2ba &&, rxv21Rx2dbFilled)
{
    rx2ba = rx2db;
	rx2cs &= ~RX_TR;
	fillBuffer ();
    return rxv21Idle {};
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
		// ToDo: Remove rx2ba access via bus
		buffer[ptr] = bus->read(rx2ba);
		rx2ba += 2;
	}

	done ();
}

