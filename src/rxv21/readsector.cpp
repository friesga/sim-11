#include "rxv21.h"

#include <cstring>

// After function selection the Read Sector command requires
// two additional steps:
// 1. Loading of the EX2DB register with the desired sector address (RX2SA),
// 2. Loading of the EX2DB with the desired track address (RX2TA). This will
//	  start the command execution.
// 
// Note that the TR bit is not deasserted and asserted in between
// steps.
void RXV21::entry (rxv21ReadSectorRx2sa)
{
	clearErrors();
    rx2cs &= ~RX_DONE;
    rx2cs |= RX_TR;
    rx2es = RX2ES_DRV_RDY | RX2ES_DRV_DEN;
}

State RXV21::transition (rxv21ReadSectorRx2sa &&, rxv21Rx2dbFilled)
{
	/* read RX2SA */
	rx2sa = rx2db;
	return rxv21ReadSectorRx2ta {};
}

State RXV21::transition (rxv21ReadSectorRx2ta &&, rxv21Rx2dbFilled)
{
	/* read RX2TA */
	rx2ta = rx2db;
	rx2cs &= ~RX_TR;
	readSector ();
	return rxv21Idle {};
}

// This function locates the desired sector and transfers the contents
// of the data field to the internal buffer in the control. This function may
// also be used to retrieve rapidly (5 ms) the current status of the drive
// selected.
void RXV21::readSector ()
{
	u32 offset;

	rx2sa &= 0037;
	rx2ta &= 0177;

	offset = (rx2sa - 1) * 256 + rx2ta * (26 * 256);

	trc.rxv21Command ((rx2cs & RX_FUNCTION_MASK) >> 1, rx2cs);
	trc.rxv21Disk (RXV21DiskCmd::RXV21_READ, (rx2cs & RX_UNIT_SEL) ? 1 : 0,
			(rx2cs & RX_DEN) ? 1 : 0, rx2sa, rx2ta);

	if (!(rx2cs & RX_DEN)) 
	{
		trc.rxv21Error (RXV21ErrorRecordType::RXV21_DEN_ERR, 0);
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
			trc.rxv21Error (RXV21ErrorRecordType::RXV21_TRACK_NO, rx2ta);
		} 
		else 
		{
			error = 0070; /* Desired sector could not be found after looking at 52 headers (2 revolutions) */
			trc.rxv21Error (RXV21ErrorRecordType::RXV21_SECT_NO, rx2ta);
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