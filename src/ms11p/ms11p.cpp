#include "ms11p.h"
#include "absoluteloader/absoluteloader.h"
#include "trace/trace.h"

#include <bit>
#include <iostream>
#include <iomanip>
#include <sstream>

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

	syndromeBits_ = generateSyndromeBits (storedCheckBits, generatedCheckBits);

	return detectedErrors (syndromeBits_);
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