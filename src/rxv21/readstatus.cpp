#include "rxv21.h"

// The Maintenance Read Status command has the following effect:
// - Done is cleared. 
// - The Drive Ready bit (bit 7 RX2ES) is updated by counting index pulses in
//   the control. 
// - The Drive Density is updated by loading the head of the selected drive and
//   reading the first data mark. 
// - The RX2ES is moved into the RX2DB.
void RXV21::readStatus ()
{
	TRCRXV21CMD ((rx2cs & RX_FUNCTION_MASK) >> 1, rx2cs);

	rx2es |= RX2ES_DRV_RDY | RX2ES_DRV_DEN;
	done ();
}