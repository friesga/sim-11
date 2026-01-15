#include "ky11_a.h"

// The KY11-A allows reading of the switch register via the bus
// at address 0177570.
//
CondData<u16> KY11_A::read (BusAddress address)
{
    return static_cast<u16> (*switchRegister_);
}

StatusCode KY11_A::writeWord (BusAddress address, u16 value)
{
    return StatusCode::NonExistingMemory;
}

bool KY11_A::responsible (BusAddress address)
{
    return address == switchRegisterAddress;
}

void KY11_A::reset ()
{}