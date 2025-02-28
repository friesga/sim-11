#include "ms11p.h"
#include "absoluteloader/absoluteloader.h"
#include "trace/trace.h"

#include <bit>

using std::shared_ptr;
using std::bind;
using std::placeholders::_1;
using std::popcount;

MS11P::MS11P (Bus* bus)
	:
	bus_ {bus}
{
	// Allocate zero-initialized memory
	memory_ = make_unique<u16[]> (memorySize_.wordCapacity ());
	checkBits_= make_unique<u8[]> (memorySize_.wordCapacity ());

	bus_->BPOK ().subscribe (bind (&MS11P::BPOKReceiver, this, _1));
}

MS11P::MS11P (Bus* bus, shared_ptr<MS11PConfig> ms11pConfig)
	:
	MS11P (bus)
{
	powerSource_ = ms11pConfig->powerSource;
	startingAddress_ = ms11pConfig->startingAddress;
	csrAddress_ = ms11pConfig->csrAddress;
}

MS11P::~MS11P ()
{}

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

// Compare the stored checkbits with the newly generated check bits and return
// a single or double bit error if they differ. In that case the syndrome bits
// and error address are stored in the error log.
//
// In case an address is read before it has been written the stored check
// bits are zero and the those check bits cannot be compared with the
// generated check bits.
MS11P::BitError MS11P::checkParity (BusAddress address, u8 storedCheckBits,
	u8 generatedCheckBits)
{
	if ((storedCheckBits == 0) || (storedCheckBits == generatedCheckBits))
        return BitError::None;

	u8 syndromeBits = generateSyndromeBits (storedCheckBits, generatedCheckBits);

	return popcount (syndromeBits) == 1 ?
		BitError::Single : BitError::Double;
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

// If bit 1 is set to 1, you can clear a soft double error in memory
// by writing new data into one or both PDP-11 memory locations of the
// bad 22-bit word. (EK-MS11P-TM-001 par 2.3.10)
//
StatusCode MS11P::writeWord (BusAddress address, u16 value)
{
	if (address.isInIOpage () && (address.registerAddress () == csrAddress_))
		writeCSR (value);
	else
	{
		memory_[(address >> 1) - startingAddress_] = value;
		checkBits_[(address >> 1) - startingAddress_] = 
			newCheckBits (address, value);

		// trace.ms11_p (MS11_PRecordType::WriteMemory, csr_.value, address, value,
		//	newCheckBits (address, value));
	}

	return StatusCode::Success;
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

// The MS11-P is responsible for its CSR and for its memory
bool MS11P::responsible (BusAddress address)
{
	if (address.isInIOpage ())
        return address.registerAddress () == csrAddress_;
    else
        return address >= startingAddress_ &&
            address < startingAddress_ + memorySize_.byteCapacity ();
}

u16 MS11P::loadFile (const char* fileName)
{
	return AbsoluteLoader::loadFile (fileName, reinterpret_cast<u8*> (memory_.get ()));
}

void MS11P::reset ()
{}

// If power is lost and no battery backup is available the memory is cleared.
void MS11P::BPOKReceiver (bool signalValue)
{
	if (!signalValue && !bus_->BatteryPower ())
	{
		memory_ = make_unique<u16[]> (memorySize_.wordCapacity ());
		checkBits_= make_unique<u8[]> (memorySize_.wordCapacity ());
    }
}