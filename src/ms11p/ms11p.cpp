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
				// Clear error log
				if (csr_.diagnosticCheck && !inhibited (address))
					csr_.checkBitsStorage = storedCheckBits;
				errorLog_.syndromeBits = 0;
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

	errorLog_.address = address;
	errorLog_.syndromeBits =
		generateSyndromeBits (storedCheckBits, generatedCheckBits);

	return popcount (errorLog_.syndromeBits) == 1 ?
		BitError::Single : BitError::Double;
}

CondData<u16> MS11P::readCSR ()
{
	// A one is read in CSR bit 11 if CSR bits 2, 23 and 14 are set to indicate
	// that the memory under test is an MS11-P. (EK-MS11P-TM-001)
	if (csr_.diagnosticCheck && csr_.inhibitModeEnable &&
			csr_.eubErrorAddressRetrieval)
		csr_.a17 = 1;

	// When a single error has occurred the highest order bits of the error
	// address can be read from the CSR by setting bit 14 (EUB Address
	// retrieval) and clearing bit 2 (diagnostic check). To read the syndrome
	// bits both bit 14 and bit 2 must be set to 1. 
	if (errorLog_.syndromeBits != 0 && csr_.eubErrorAddressRetrieval)
	{
		if (!csr_.diagnosticCheck)
			csr_.errorAddressStorage = addressBitsA21_A18 (errorLog_.address);
		else
			csr_.checkBitsStorage = errorLog_.syndromeBits;
	}

	// trace.ms11_p (MS11_PRecordType::ReadCSR, csr_.value, 0, 0, 0);
	return {csr_.value};
}


StatusCode MS11P::writeWord (BusAddress address, u16 value)
{
	if (address.isInIOpage () && (address.registerAddress () == csrAddress_))
		writeCSR (value & writeMask);
	else
	{
		memory_[(address >> 1) - startingAddress_] = value;
		checkBits_[(address >> 1) - startingAddress_] = 
			newCheckBits (address, value);
	}

	// trace.ms11_p (MS11_PRecordType::WriteMemory, csr_.value, address, value,
	//	newCheckBits (address, value));
	return StatusCode::Success;
}

// Bit 13 of the CSR is "not used" on the MS11-L and is read/write on the
// MS11-M and MS11-P. This is used by diagnostics to discriminate between
// ECC modules (MS11-M and MS11-P) and non-ECC modules (MS11-L).
//
void MS11P::writeCSR (u16 value)
{
	csr_.value = value;
	// trace.ms11_p (MS11_PRecordType::WriteCSR, value, 0, 0, 0);
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