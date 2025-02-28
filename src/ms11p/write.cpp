#include "ms11p.h"
#include "trace/trace.h"

// If bit 1 is set to 1, you can clear a soft double error in memory
// by writing new data into one or both PDP-11 memory locations of the
// bad 22-bit word. (EK-MS11P-TM-001 par 2.3.10)
//
StatusCode MS11P::writeWord (BusAddress address, u16 value)
{
	if (address.isInIOpage () && (address.registerAddress () == csrAddress_))
		writeCSR (value);
	else
		writeMemory (address, value);

	return StatusCode::Success;
}

void MS11P::writeMemory (BusAddress address, u16 value)
{
	memory_[(address >> 1) - startingAddress_] = value;
	checkBits_[(address >> 1) - startingAddress_] =
		newCheckBits (address, value);

	// trace.ms11_p (MS11_PRecordType::WriteMemory, csr_.value, address, value,
	//	newCheckBits (address, value));
}

// Bit 13 of the CSR is "not used" on the MS11-L and is read/write on the
// MS11-M and MS11-P. This is used by diagnostics to discriminate between
// ECC modules (MS11-M and MS11-P) and non-ECC modules (MS11-L).
//
// The MS11-P uses six bytes for parity. This implies that CSR bit 11 isn't
// used for parity and that bit cannot be written. This feature is used by
// ZMSPC0 to discriminate a MS11-P from other memory modules.
//
// Take care that if (the Check Bit Storage in) the CSR is written and read
// back immediately the CSR should contain the just written check bits and
// should not contain the check bits from a previous memory access in
// diagnostic mode. This feature too is used to discriminate a MS11-P from
// other memory modules.
// 
// Note that a DATO cycle to the CSR destroys the retrieved check bits, but
// an error address recorded in the CSR is preserved. (EK-MS11P-TM-001,
// par 2.3.10)
//
// The EUB address A21 through A18 [in fact the Error Address/Syndrome Bits
// storage] is read only when CSR 14 equals 1. (EK-MS11P-TM-001, par. 2.3.6.3)
// See also the CSR examples in par. 2.3.11.
//
void MS11P::writeCSR (u16 value)
{
	trace.ms11_p (MS11_PRecordType::WriteCSR, csr_.value, 0, value, checkSyndromeBits_);

	bool syndromeBitsProtected = (csr_.uncorrectableErrorIndication
		|| csr_.singleErrorIndication) && !(value & 02);

	// Set all CSR bits to to the given value except for the syndrome storage	
	// and bit 13
	csr_.value = (csr_.value & checkBitStorageMask) |
		(value & writeMask & ~checkBitStorageMask);

	// Next determine if the syndrome storage must be written
	if (csr_.diagnosticCheck && !csr_.eubErrorAddressRetrieval &&
		!syndromeBitsProtected)
	{
		checkSyndromeBits_ = (value & checkBitStorageMask) >> 5;
		csr_.errorAddressStorage = (value & checkBitStorageMask) >> 5;
		trace.debug ("checkSyndromeBits set: " + to_string (checkSyndromeBits_));
	}
}
