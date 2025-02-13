#include "ms11p.h"

CondData<u16> MS11P::handleSingleError (BusAddress address, u16 data,
	u8 storedCheckBits, u8 generatedCheckBits)
{
	// The Single Error bit is set unless Disable Correction Mode is set
	// and the error address is inhibited.
	csr_.singleErrorIndication =
		csr_.errorCorrectionDisable && inhibited (address) ? 0 : 1;

	// The Double Error bit is set when Disable Correction Mode is set
	// and the error address is not inhibited.
	csr_.uncorrectableErrorIndication = 
		csr_.errorCorrectionDisable && !inhibited (address) ? 1 : 0;

	// The CSR ErrorAddress and Check Bit Storage contains the check bits
	// when the Diagnostic Mode is set and the error address is not inhibited.
	if (csr_.diagnosticCheck && !inhibited (address))
		csr_.errorAddressAndCheckBits = storedCheckBits;

	// The CSR ErrorAddress and Check Bit Storage contains bits A17 - A11
	// of the error address when in Normal Mode (i.e. Diagnostic Mode and 
	// Disable Correction Mode both not set) or Disable Correction mode is
    // set and the error address is not inhibited.
	if ((!csr_.diagnosticCheck && !csr_.errorCorrectionDisable) ||
		csr_.errorCorrectionDisable && !inhibited (address))
	{
		errorLog_.address = address;
        errorLog_.syndromeBits = 
			generateSyndromeBits (storedCheckBits, generatedCheckBits);
		csr_.errorAddressAndCheckBits = addressBitsA17_A11 (address);
	}

	return {data};
}

u8 MS11P::addressBitsA17_A11 (BusAddress address)
{
    return (address >> 11) & 0177;
}

u8 MS11P::addressBitsA21_A18 (BusAddress address)
{
	return (address >> 18) & 0177;
}

// The syndrome bits are produced by an exclusivr or of the two sets of
// checkbits. If the two sets of check bits are identical (there are no
// errors) the syndrome bits are all zeros.
u8 MS11P::generateSyndromeBits (u8 checkBitSet1, u8 checkBitset2)
{
	return checkBitSet1 ^ checkBitset2;
}