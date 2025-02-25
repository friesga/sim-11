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

	// In Diagnostic/unprotected (c.q. not inhibited) mode the check bits
	// read from memory are logged in the Check Bit Storage.
	if (csr_.diagnosticCheck && !inhibited (address))
	{
		checkSyndromeBits_ = storedCheckBits;
		checkSyndromeBitsState_ = CheckSyndromeBitsState::SourceMemory;
	}
	else
	{
		errorAddress_ = address;
		checkSyndromeBits_ = 
			generateSyndromeBits (storedCheckBits, generatedCheckBits);
		checkSyndromeBitsState_ = CheckSyndromeBitsState::SourceMemory;
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