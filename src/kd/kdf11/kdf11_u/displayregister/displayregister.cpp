#include "displayregister.h"

StatusCode DisplayRegister::read (BusAddress<> address, u16* destination)
{
    return StatusCode::NonExistingMemory;
}

StatusCode DisplayRegister::writeWord (BusAddress<> address, u16 value)
{
    displayRegister_ = value;
    return StatusCode::OK;
}

bool DisplayRegister::responsible (BusAddress<> address)
{
    return address.isInIOpage () && 
        address.registerAddress () == displayRegister;
}

void DisplayRegister::reset ()
{}