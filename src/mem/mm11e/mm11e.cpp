#include "types.h"
#include "mm11e.h"
#include "absoluteloader/absoluteloader.h"

#include <string.h>
#include <stdlib.h>

using std::bind;
using std::placeholders::_1;

// The MM11-E is a 16-bit, 4096 word (8KB) core memory module.
//
MM11E::MM11E (Bus* bus)
	:
	bus_ {bus},
	startingAddress_ {0},
	memorySizeKB_ {8192},
	data {new u8[memorySizeKB_] {}}
{ }

MM11E::MM11E (Bus* bus, const MM11EConfig& mm11eConfig)
	:
	bus_ {bus},
	startingAddress_ {mm11eConfig.startingAddress},
	memorySizeKB_ {mm11eConfig.memorySizeKB},
	data {new u8[memorySizeKB_] {}}
{ }

MM11E::~MM11E ()
{
	delete[] data;
}

CondData<u16> MM11E::read (BusAddress address)
{
	// Get the contents of the specified address via a u16 pointer
	// as data is an array of bytes.
	u16* mem = (u16*) &data[address - startingAddress_];
	return {*mem};
}

StatusCode MM11E::writeWord (BusAddress address, u16 value)
{
	u16* mem = (u16*) &data[address - startingAddress_];
	*mem = value;

	return StatusCode::Success;
}

bool MM11E::responsible (BusAddress busAddress)
{
	return !busAddress.isInIOpage () && busAddress >= startingAddress_ &&
		busAddress < (startingAddress_ + memorySizeKB_);
}

void MM11E::reset ()
{
	/* nothing */
}

u16 MM11E::loadFile (const char* fileName)
{
	return AbsoluteLoader::loadFile (fileName, data);
}
