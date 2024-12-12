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
    return addressInDiagnosticROM (address) || addressInBootRom (address);
}

// This function returns the data at the specified address from the ROM image
// (if present). The address is a byte address and as the ROM images are
// defined as word arrays the byte address has to be converted to a word
// address (by dividing it by two).
StatusCode M9312::read (BusAddress busAddress, u16* data)
{
    if (addressInDiagnosticROM (busAddress))
        return readDiagnosticROM (busAddress, data);

    if (addressInBootRom (busAddress))
        return readBootROM (busAddress, data);

    return StatusCode::NonExistingMemory;
}

StatusCode M9312::writeByte (BusAddress busAddress, u8 data)
{
    return StatusCode::NonExistingMemory;
}

StatusCode M9312::writeWord (BusAddress busAddress, u16 data)
{
    return StatusCode::NonExistingMemory;
}

bool M9312::addressInDiagnosticROM (BusAddress address)
{
    return address.isInIOpage () && 
           address.registerAddress () >= diagROMBaseAddress &&
           address.registerAddress () < diagROMBaseAddress + diagROMSize * 2;
}

bool M9312::addressInBootRom (BusAddress address)
{
    return address.isInIOpage () &&
           address.registerAddress () >= bootROMBaseAddress &&
           address.registerAddress () < bootROMBaseAddress +
                bootROMSize * 2 * numberOfBootROMs;
}

StatusCode M9312::readDiagnosticROM (BusAddress busAddress, u16* data)
{
    if (diagnosticROM_ == nullptr)
        return StatusCode::NonExistingMemory;

    u16 imageIndex = busAddress.registerAddress () - diagROMBaseAddress >> 1;
    *data = (*diagnosticROM_)[imageIndex];
    return StatusCode::OK;
}

StatusCode M9312::readBootROM (BusAddress busAddress, u16* data)
{
    u16 romNumber = getBootRomNumber (busAddress);

    if (bootROM_[romNumber] == nullptr)
        return StatusCode::NonExistingMemory;

    u16 imageIndex = busAddress.registerAddress () - 
        bootROMBaseAddresses[romNumber] >> 1;
    *data = (*bootROM_[romNumber])[imageIndex];
    return StatusCode::OK;
}