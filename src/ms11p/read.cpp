#include "ms11p.h"
#include "trace/trace.h"

CondData<u16> MS11P::read (BusAddress address)
{
	if (address.isInIOpage () && (address.registerAddress () == csrAddress_))
		return readCSR ();
	else
	{
		// Get the contents of the memory location converting the byte address
		// to an index into the word array (which is allowed as all addresses
		// are even word addresses).
		u16 data = memory_[(address >> 1) - startingAddress_];
		u8 storedCheckBits = checkBits_[(address >> 1) - startingAddress_];
		u8 generatedCheckBits = generateCheckBits (data);

		// trace.ms11_p (MS11_PRecordType::ReadMemory, csr_.value, address, data,
		//	storedCheckBits);

		switch (checkParity (address, storedCheckBits, generatedCheckBits))
		{
			case BitError::None:
				if (csr_.diagnosticCheck && !inhibited (address))
					checkSyndromeBits_ = storedCheckBits;
				return {data};

			case BitError::Single:
				return handleSingleError (address, data, storedCheckBits,
					generatedCheckBits);

			case BitError::Double:
				return handleDoubleError (address, data, storedCheckBits,
					generatedCheckBits);
		}
	}

	// Satisfy the compiler
	throw "Should not happen";
}

// If during normal operation, a double or single error occurs during a DATI,
// DATIP or DATAOB cycle, and if CSR bit 2 is set to zero [i.e. no diagnostic
// mode set], CSR bits 5 through 10 store syndrome bits X, 0, 1, 2, 4, and 8.
// To read syndrome bits from the CSR, CSR bit 2 must be set to 1 (diagnostic
// mode) and the CSR read. This operation allows syndrome bits for a single
// or double failure to be read, instead of the address bits normally read
// when CSR 2 is set to zero.
//
// When a single error has occurred the highest order bits of the error
// address can be read from the CSR by setting bit 14 (EUB Address
// retrieval) and clearing bit 2 (diagnostic check). To read the syndrome
// bits both bit 14 and bit 2 must be set to 1. 
// Source: EK-MS11P-TM-001
//
// This leads to the following table (see EK-MS11P-TM-001, Table 3-5):
//
// EUB Address (14) | Diag check (2)  | Storage contents |
// -----------------+-----------------+------------------+
//  	0			|	 0   		  |	A11-A17		     |
// 		1			|	 0			  |	A18-A21          |
//		0			|	 1			  |	Syndrome bits    |
//		1			|	 1			  |	Syndrome bits    |
// -----------------+-----------------+------------------+
//
// Note that in Normal Operating Mode (no read errors) with diagnostic check
// enabled, the Check Bit Storage is written when the memory is read.
// 
// When the memory is not in diagnostic mode (CSR bit 2 equals 0), data
// previously loaded into CSR bits 11-5 cannot be read. (EK-MS11P-TM-001,
// par 2.3.10)
//
CondData<u16> MS11P::readCSR ()
{
	// A one is read in CSR bit 11 if CSR bits 2, 23 and 14 are set to indicate
	// that the memory under test is an MS11-P. (EK-MS11P-TM-001 par. 2.3.6.1)
	if (csr_.diagnosticCheck && csr_.inhibitModeEnable &&
		csr_.eubErrorAddressRetrieval)
	{
		csr_.a17 = 1;
		trace.ms11_p (MS11_PRecordType::ReadCSR, csr_.value, 0, 0, 0);
		return {csr_.value};
	}

	if (csr_.diagnosticCheck)
		csr_.errorAddressStorage = checkSyndromeBits_;
	else
	{
		if (csr_.singleErrorIndication || csr_.uncorrectableErrorIndication)
		{
			if (csr_.eubErrorAddressRetrieval)
				csr_.errorAddressStorage = addressBitsA21_A18 (errorAddress_);
			else
				csr_.errorAddressStorage = addressBitsA17_A11 (errorAddress_);
		}
	}

	trace.ms11_p (MS11_PRecordType::ReadCSR, csr_.value, 0, 0, 0);
	return {csr_.value};
}
