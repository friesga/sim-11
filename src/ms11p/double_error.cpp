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
			csr_.errorAddressAndCheckBits = storedCheckBits;
		return {data, StatusCode::ParityError};
	}
	else
	{
		csr_.errorAddressAndCheckBits = addressBitsA17_A11 (address);
		if (csr_.uncorrectableErrorIndication)
			return {data, StatusCode::ParityError};
		else
			return {data};
	}
}