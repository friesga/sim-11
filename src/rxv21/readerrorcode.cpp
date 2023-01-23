#include "rxv21.h"

// After function selection the Write Sector command requires
// one additional step:
// 1. Loading of the EX2DB register with the bus adress. This will
//	  start the command.
void RXV21::entry (rxv21ReadErrorCodeRx2ba)
{
	rx2cs &= ~RX_DONE;
    rx2cs |= RX_TR;
    rx2es = RX2ES_DRV_RDY | RX2ES_DRV_DEN;
}

State RXV21::transition (rxv21ReadErrorCodeRx2ba &&, rxv21Rx2dbFilled)
{
	rx2cs &= ~RX_TR;
	readErrorCode ();
	return rxv21Idle {};
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

	trc.rxv21Command ((rx2cs & RX_FUNCTION_MASK) >> 1, rx2cs);

	/* < 7:0> Definitive Error Codes */
	/* <15:8> Word Count Register */
	bus->writeWord (rx2ba, error | (rx2wc << 8));

	/* < 7:0> Current Track Address of Drive 0 */
	/* <15:8> Current Track Address of Drive 1 */
	bus->writeWord (rx2ba + 2, rx2ta | (rx2ta << 8));

	/* < 7:0> Target Track of Current Disk Access */
	/* <15:8> Target Sector of Current Disk Access */
	bus->writeWord (rx2ba + 4, rx2ta | (rx2sa << 8));

	/* <7> Unit Select Bit */
	/* <5> Head Load Bit */
	/* <6> <4> Drive Density Bit of Both Drives */
	/* <0> Density of Read Error Register Command */
	/* <15:8> Track Address of Selected Drive */
	bus->writeWord (rx2ba + 6, _BV(5) | _BV(6) | _BV(4) | _BV(0) | (rx2ta << 8));

	done ();
}

