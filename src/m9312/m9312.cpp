#include "m9312.h"
#include "qbus/qbus.h"

using std::bind;
using std::placeholders::_1;

M9312::M9312 (Qbus* bus, shared_ptr<M9312Config> m9312Config)
    :
    PDP11Peripheral (bus)
{
    diagnosticROM_ = diagROMImages_[+m9312Config->diagnosticROM];

    for (size_t socketNr = 0; socketNr < numberOfBootROMs; ++socketNr)
        bootROM_[socketNr] = bootROMImages_[+m9312Config->bootROM[socketNr]];

    startingAddress_ = m9312Config->startingAddress;

    // Make sure the m9312 is notified of power-up's so it can "steal" the
    // powerfail vector if needed.
    bus_->BPOK ().subscribe (bind (&M9312::BPOKReceiver, this, _1));
}

void M9312::reset ()
{}

// Ony if the first two reads to after a power-up are for the powerfail
// vector and no battery backup is provided, and the power-up boot is enabled,
// the M9312 "steals" the read from memory and returns the vector as provided
// in the boot ROM in slot 0. Any read to an address other than the powerfail
// vector indicates that no boot via the powerfail vector is in progress.
bool M9312::responsible (BusAddress address)
{
    if (addressIsPowerfailVector (address) && powerUpViaVector_)
        return true;
    else
    {
        powerUpViaVector_ = false;
        return addressInDiagnosticROM (address) || addressInBootRom (address);
    }
}

// This function returns the data at the specified address from the ROM
// image (if present). The address is a byte address and as the ROM images
// are defined as word arrays the byte address has to be converted to a
// word address (by dividing it by two).
//
// If the system is powered up via the powerfailvector, no battery backup is
// provided and the power-up-boot is enabled, reads from the powerfail vector
// are taken care of by the M9312, resulting in a cold boot. In this case
// the starting address is the address specified in the address offset
// switch bank.
// 
StatusCode M9312::read (BusAddress busAddress, u16* data)
{
    if (addressIsPowerfailVector (busAddress))
        busAddress += bootROMBaseAddress;

    if (busAddress == addressOffsetSwitchBankAddress)
        return readAddressOffsetSwitchBank (data);

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

bool M9312::addressIsPowerfailVector (BusAddress address)
{ 
    return (address == 024 || address == 026);
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

StatusCode M9312::readAddressOffsetSwitchBank (u16* data)
{
    *data = startingAddress_;
    return StatusCode::OK;
}

// On power down switch off the line time clock.
void M9312::BPOKReceiver (bool signalValue)
{
    if (signalValue)
        powerUpViaVector_ = true;
}