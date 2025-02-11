#include "ms11p.h"

void MS11P::handleDoubleError (BusAddress address, u8 storedCheckBits,
	u8 generatedCheckBits)
{
	// Set Double Error bit
	csr_.uncorrectableErrorIndication = 1;

	// When the Diagnostic Mode is set and the error address is not inhibited
	// the CSR ErrorAddress and Check Bit Storage contains the check bits.
	// In all other cases the storage contains bits A17 through A11 of the
	// error address.
	if (csr_.diagnosticCheck && !inhibited (address))
		csr_.errorAddressAndCheckBits = storedCheckBits;
	else
		csr_.errorAddressAndCheckBits = addressBitsA17_A11 (address);


}