#include "rxv21.h"

#include <cstring>

using namespace rxv21;

// After function selection the Write Sector command requires
// two additional steps:
// 1. Loading of the EX2DB register with the desired sector address (RX2SA),
// 2. Loading of the EX2DB with the desired track address (RX2TA). This will
//	  start the command execution.
// 
// Note that the TR bit is not deasserted and asserted in between
// steps.
void RXV21::entry (rxv21WriteSectorRx2sa)
{
	clearErrors();
    rx2cs &= ~RX_DONE;
    rx2cs |= RX_TR;
    rx2es = RX2ES_DRV_RDY | RX2ES_DRV_DEN;
}

State RXV21::transition (rxv21WriteSectorRx2sa &&, rxv21Rx2dbFilled)
{
	/* read RX2SA */
	rx2sa = rx2db;
	return rxv21WriteSectorRx2ta {};
}

State RXV21::transition (rxv21WriteSectorRx2ta &&, rxv21Rx2dbFilled)
{
	/* read RX2TA */
	rx2ta = rx2db;
	rx2cs &= ~RX_TR;
	writeSector ();
	return rxv21Idle {};
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

	trace.rxv21Command ((rx2cs & RX_FUNCTION_MASK) >> 1, rx2cs);
	trace.rxv21Disk (RXV21DiskCmd::RXV21_WRITE, (rx2cs & RX_UNIT_SEL) ? 1 : 0,
			(rx2cs & RX_DEN) ? 1 : 0, rx2sa, rx2ta);

	if (!(rx2cs & RX_DEN)) 
	{
		trace.rxv21Error (RXV21ErrorRecordType::RXV21_DEN_ERR, 0);
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
			trace.rxv21Error (RXV21ErrorRecordType::RXV21_TRACK_NO, rx2ta);
		} 
		else 
		{
			error = 0070; /* Desired sector could not be found after looking at 52 headers (2 revolutions) */
			trace.rxv21Error (RXV21ErrorRecordType::RXV21_SECT_NO, rx2sa);
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