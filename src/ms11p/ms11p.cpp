#include "ms11p.h"
#include "absoluteloader/absoluteloader.h"

using std::shared_ptr;
using std::bind;
using std::placeholders::_1;

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

StatusCode MS11P::read (BusAddress address, u16* destAddress)
{
	if (address.registerAddress () == csrAddress_)
        readCSR (destAddress);
	else
	{
		// Get the contents of the memory location converting the byte address
		// to an index into the word array (which is allowed as all addresses
		// are even word addresses).
        *destAddress = memory_[(address >> 1) - startingAddress_];
	}
	return StatusCode::OK;
}

void MS11P::readCSR (u16* destAddress)
{
	// To read syndrome bits from the CSR, CSR bit 2 must be set to 1
	// (diagnostic mode) and the CSR read. (EK-MS11P-TM-001)
	if (csr_.diagnosticCheck)
		csr_.errorAddressAndCheckBits = syndromeBits_;

	// A one is read in CSR bit 11 if CSR bits 2, 23 and 14 are set to indicate
	// that the memory under test is an MS11-P. (EK-MS11P-TM-001)
	if (csr_.diagnosticCheck && csr_.inhibitModeEnable &&
			csr_.eubErrorAddressRetrieval)
		csr_.a17 = 1;

	*destAddress = csr_.value;
}


StatusCode MS11P::writeWord (BusAddress address, u16 value)
{
	if (address.registerAddress () == csrAddress_)
		writeCSR (value);
	else
	{
		memory_[(address >> 1) - startingAddress_] = value;
	}

	return StatusCode::OK;
}

// Bit 13 of the CSR is "not used" on the MS11-L and is read/write on the
// MS11-M and MS11-P. This is used by diagnostics to discriminate between
// ECC modules (MS11-M and MS11-P) and non-ECC modules (MS11-L).
//
void MS11P::writeCSR (u16 value)
{
	csr_.value = value;
}

// The MS11-P is responsible for its CSR and for its memory
bool MS11P::responsible (BusAddress address)
{
	return address.registerAddress () == csrAddress_ ||
		(!address.isInIOpage () && 
		   address >= startingAddress_ &&
		   address < startingAddress_ + memorySize_.byteCapacity ());
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