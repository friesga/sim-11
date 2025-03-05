#include "ms11p.h"

// For single errors, the error correction logic complements (corrects)
// the single bit in error. [...] If the single error is one of the check
// bits, the correction logic does not place corrected check bits on the
// syndrome/check bit outputs. EK-MS11P-TM-001, p. 3-12.
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
		checkSyndromeBits_ = storedCheckBits;
	else
	{
		errorAddress_ = address;
		checkSyndromeBits_ = syndromeBits_;
	}

	// Try to correct the error
	if (!csr_.errorCorrectionDisable && !inhibited (address))
	{
		if (syndromeDecodeTable_ [syndromeBits_].key == 
				SyndromeDecodeKey::DataBit)
			data = correctSingleError (data, syndromeBits_);
	} 
	else if (csr_.uncorrectableErrIndicationEnable)
		return {data, StatusCode::ParityError};

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

// The syndrome bits are produced by an exclusive or of the two sets of
// checkbits. If the two sets of check bits are identical (there are no
// errors) the syndrome bits are all zeros. (EK-MS11P-TM-001, par. 3.2.2)
u8 MS11P::generateSyndromeBits (u8 checkBitSet1, u8 checkBitset2)
{
	return checkBitSet1 ^ checkBitset2;
}
