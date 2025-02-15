#include "ms11p.h"

CondData<u16> MS11P::handleDoubleError (BusAddress address, u16 data,
	u8 storedCheckBits, u8 generatedCheckBits)
{
	// Set Double Error bit
	csr_.uncorrectableErrorIndication = 1;

	// When the Diagnostic Mode is set and the error address is not inhibited
	// the CSR ErrorAddress and Check Bit Storage contains the check bits.
	// In all other cases the storage contains bits A17 through A11 of the
	// error address.
	if (csr_.diagnosticCheck)
	{
		if (!inhibited (address))
			csr_.checkBitsStorage = storedCheckBits;
	}
	else
		csr_.errorAddressStorage = addressBitsA17_A11 (address);

	// PIf a double error coccurs with ECC enabled, or a single or double
	// error occurs with ECC disabled and bit 0 [Uncorrectable Error Indicator
	// Enable] set, then during a DATI or DATIP cycle to memory, BUS PB L
	// is asserted on the Unibus [generating a parity error trap] at the
	// same time as data. (EK-MS11P-TM-001, par. 2.3.1).
	if (csr_.uncorrectableErrIndicationEnable)
		return {data, StatusCode::ParityError};
	else
		return {data};
}