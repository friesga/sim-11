#include "m9312.h"

M9312::M9312 (Qbus* bus, shared_ptr<M9312Config> m9312Config)
    :
    PDP11Peripheral (bus)
{
    diagnosticROM_ = diagROMImages_[+m9312Config->diagnosticROM];

    for (size_t socketNr = 0; socketNr < numberOfBootROMs; ++socketNr)
        bootROM_[socketNr] = bootROMImages_[+m9312Config->bootROM[socketNr]];

    startingAddress_ = m9312Config->startingAddress;
}

void M9312::reset ()
{}

bool M9312::responsible (BusAddress address)
{
    return address >= diagROMBaseAddress &&
           address < diagROMBaseAddress + diagROMSize * 2 ||
           address >= bootROMBaseAddress &&
           address < bootROMBaseAddress + bootROMSize * 2 * numberOfBootROMs;
}

StatusCode M9312::read (BusAddress busAddress, u16* data)
{
    if (busAddress >= diagROMBaseAddress &&
        busAddress < diagROMBaseAddress + diagROMSize * 2)
    {
        if (diagnosticROM_ == nullptr)
            return StatusCode::NonExistingMemory;

        u16 address = busAddress - diagROMBaseAddress;
        *data = (*diagnosticROM_)[address];
        return StatusCode::OK;
    }

    if (busAddress >= bootROMBaseAddress &&
        busAddress < bootROMBaseAddress + bootROMSize * 2 * numberOfBootROMs)
    {
        u16 romNumber = getBootRomNumber (busAddress);

        if (bootROM_[romNumber] == nullptr)
            return StatusCode::NonExistingMemory;

        u16 address = busAddress - bootROMBaseAddresses[romNumber];
        *data = (*bootROM_[romNumber])[address];
        return StatusCode::OK;
    }
    return StatusCode::NonExistingMemory;
}

StatusCode M9312::writeByte (BusAddress busAddress, u8 data)
{
    return StatusCode::FunctionNotImplemented;
}

StatusCode M9312::writeWord (BusAddress busAddress, u16 data)
{
    return StatusCode::FunctionNotImplemented;
}