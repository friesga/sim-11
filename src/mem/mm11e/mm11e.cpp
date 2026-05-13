#include "types.h"
#include "mm11e.h"
#include "absoluteloader/absoluteloader.h"

#include <string.h>
#include <stdlib.h>

using std::make_unique;

// The MM11-E is a 16-bit, 4096 word (8KB) core memory module. The MM11-EX
// is an extended version of the MM11-E with 8192 words (16KB) of memory. 
//
MM11E::MM11E (Bus* bus)
	:
	bus_ {bus},
	startingAddress_ {0},
	memorySize_ {4_KiW}
{
	memory_ = make_unique<u16[]> (memorySize_.wordCapacity ());
}

MM11E::MM11E (Bus* bus, const MM11EConfig& mm11eConfig)
	:
	bus_ {bus},
	startingAddress_ {mm11eConfig.startingAddress},
	memorySize_ {mm11eConfig.memorySize}
{
	memory_ = make_unique<u16[]> (memorySize_.wordCapacity ());
}

MM11E::~MM11E ()
{}

CondData<u16> MM11E::read (BusAddress address)
{
	u16 data = memory_[(address - startingAddress_) >> 1];
	return data;
}

StatusCode MM11E::writeWord (BusAddress address, u16 value)
{
	memory_[(address - startingAddress_) >> 1] = value;
	return StatusCode::Success;
}

bool MM11E::responsible (BusAddress busAddress)
{
	return !busAddress.isInIOpage () && 
			busAddress >= startingAddress_ &&
			busAddress < startingAddress_ + memorySize_.byteCapacity ();
}

void MM11E::reset ()
{
	/* nothing */
}

u16 MM11E::loadFile (const char* fileName)
{
	return AbsoluteLoader::loadFile (fileName,
		reinterpret_cast<u8*> (memory_.get ()));
}
