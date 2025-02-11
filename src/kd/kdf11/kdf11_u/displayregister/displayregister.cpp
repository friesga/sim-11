#include "displayregister.h"

CondData<u16> DisplayRegister::read (BusAddress address)
{
    return {StatusCode::NonExistingMemory};
}

StatusCode DisplayRegister::writeWord (BusAddress address, u16 value)
{
    displayRegister_ = value;
    return StatusCode::Success;
}

bool DisplayRegister::responsible (BusAddress address)
{
    return address.isInIOpage () && 
        address.registerAddress () == displayRegister;
}

void DisplayRegister::reset ()
{}