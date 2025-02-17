#include "ms11p.h"

CondData<u16> MS11P::handleDoubleError (BusAddress address, u16 data,
	u8 storedCheckBits, u8 generatedCheckBits)
{
	// Set Double Error bit
	csr_.uncorrectableErrorIndication = 1;

	// In Diagnostic/unprotected (c.q. not inhibited) mode the check bits
	// read from memory are logged in the Check Bit Storage. No addresses or
	// error syndromes are logged in the storage.
	if (csr_.diagnosticCheck && !inhibited (address))
	{
		csr_.checkBitsStorage = storedCheckBits;
		accessLog_.syndromeBits = 0;
	}

	// If a double error coccurs with ECC enabled, or a single or double
	// error occurs with ECC disabled and bit 0 [Uncorrectable Error Indicator
	// Enable] set, then during a DATI or DATIP cycle to memory, BUS PB L
	// is asserted on the Unibus [generating a parity error trap] at the
	// same time as data. (EK-MS11P-TM-001, par. 2.3.1).
	if (csr_.uncorrectableErrIndicationEnable)
		return {data, StatusCode::ParityError};
	else
		return {data};
}