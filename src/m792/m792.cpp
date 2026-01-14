#include "m792.h"

// The configuration is ignored for now as the only configuration item is
// the M792 option and currently the only option implemented is the M792-YB.
//
M792::M792 (Bus* bus, const M792Config& m792Config)
{ }

CondData<u16> M792::read (BusAddress address)
{
    return rom_M792_YB[(address - addressLow) >> 1];
}

StatusCode M792::writeWord (BusAddress address, u16 value)
{
    return StatusCode::NonExistingMemory;
}

bool M792::responsible (BusAddress address)
{
    // A M792 will probably only be used in unmapped systems but to be sure
    // we'll check the address is in the I/O page.
    if (!address.isInIOpage ())
        return false;

    return addressLow <= address && address <= addressHigh;
}

void M792::reset ()
{ }