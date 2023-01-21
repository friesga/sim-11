#include "rxv21.h"

// After function selection the Empty Buffer command requires
// two additional steps:
// 1. Loading of the EX2DB register with the word count (RX2WC),
// 2. Loading of the EX2DB with the bus address (RX2BA). This will
//	  start the command execution.
// 
// Note that the TR bit is not deasserted and asserted in between
// steps.

void RXV21::entry (rxv21EmptyBufferRx2wc)
{
	clearErrors();
    rx2cs &= ~RX_DONE;
    rx2cs |= RX_TR;
}

State RXV21::transition (rxv21EmptyBufferRx2wc &&, rxv21Rx2dbFilled)
{
    rx2wc = rx2db;
    return rxv21EmptyBufferRx2ba {};
}

State RXV21::transition (rxv21EmptyBufferRx2ba &&, rxv21Rx2dbFilled)
{
    rx2ba = rx2db;
	rx2cs &= ~RX_TR;
	emptyBuffer ();
    return rxv21Idle {};
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

	trc.TRCRXV21DMA (RXV21DiskCmd::RXV21_EMPTY, rx2wc, rx2ba);
	if (rx2wc > limit) 
	{
		trc.TRCRXV21Error (RXV21ErrorType::RXV21_WC_OVFL, rx2wc);
		error = 0230; /* Word count overflow */
		rx2es |= RX2ES_WC_OVFL;
		rx2cs |= RX_ERROR;
		done ();
		return;
	}

	for (wc = rx2wc, ptr = 0; wc > 0; wc--, ptr++) 
	{
		/* Transfer words */
		bus->writeWord (rx2ba, buffer[ptr]);
		rx2ba += 2;
	}
	done ();
}
