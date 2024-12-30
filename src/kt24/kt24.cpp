#include "kt24.h"

#include <iostream>

KT24::KT24 (Qbus* bus)
    :
    bus_ {bus}
{}

void KT24::enable ()
{
    enabled_ = true;
}

void KT24::disable ()
{
    enabled_ = false;
}

StatusCode KT24::read (BusAddress address, u16* destination)
{
    u16 registerAddress= address.registerAddress ();

    if (lowMappingRegister (registerAddress))
        *destination = 
            mappingRegisters_[indexFromRegisterAddress (registerAddress)].low;
    else
        *destination = 
            mappingRegisters_[indexFromRegisterAddress (registerAddress)].high;

    return StatusCode::OK;
}

StatusCode KT24::writeWord (BusAddress address, u16 value)
{
    u16 registerAddress= address.registerAddress ();

    if (lowMappingRegister (registerAddress))
        mappingRegisters_[indexFromRegisterAddress (registerAddress)].low = value;
    else
        mappingRegisters_[indexFromRegisterAddress (registerAddress)].high = value;

    return StatusCode::OK;
}

StatusCode KT24::writeByte (BusAddress address, u8 value)
{
    BusAddress wordAddress = address & 0xFFFFFFFE;
    u16 tmp;
    read (wordAddress, &tmp);
    if (address & static_cast<u32> (1))
        tmp = (tmp & 0x00FF) | (value << 8);
    else
        tmp = (tmp & 0xFF00) | value;

    return writeWord (wordAddress, tmp);
}

bool KT24::responsible (BusAddress address)
{
    return address.registerAddress () >= mappingRegistersBaseAddress &&
        address.registerAddress () < mappingRegistersBaseAddress + (mappingRegisters_.size () * 4);
}

void KT24::reset ()
{}

// Relocation expands the 18-bit Unibus address to the 22-bit main memory
// address. This allows the Unibus to access any location in main memory.
// This relocation or mapping of addresses is done by adding the contents
// of one of the mapping registers to bits (12:01) of the incoming Unibus
// address.
//
// All mapping registers in the Unibus map are 21 bits wide. A 22nd bit, which
// is not writable and is always read as zero, acts as the lowest-order bit
// for each register. Each register specifies the 22-bit base physical address
// of a 4K page residing on any word boundary in memory. The reason for using
// word boundaries is that the mapping logic does not recognize if a byte
// operation is being executed.
//
// Bits (17: 13) of the 18-bit Unibus address select a map register. The
// remaining bits (12:00) of the Unibus address are used as an offset into
// the page to which the mapping register is pointing. When an address is
// taken from the Unibus, the mapping register is automatically selected and
// the contents read out. Then the 22-bit base address contained in the
// selected map register is added to the 12-bit offset in the Unibus address
// to form the physical address. (EK-11024-TM-003)
//
// Bit 0 of the low mapping register is ignored as Qbus::writeWord() and
// Qbus::read() prevent writes to and reads from odd addresses.
//
StatusCode KT24::dmaRead (BusAddress address, u16* destination)
{
    CondData<u16> data = bus_->read (address);
    if (data.hasValue ())
    {
        *destination = data.value ();
        return StatusCode::OK;
    }

    return StatusCode::NonExistingMemory;
}

StatusCode KT24::dmaWrite (BusAddress address, u16 value)
{
    return enabled_ ? mappedWrite (address, value) :
        writePhysical (address, value);
}

StatusCode KT24::mappedWrite (BusAddress address, u16 value)
{
    return writePhysical (physicalAddressFrom18BitBusAddress (address),
        value);
}

// StatusCode KT24::mappedRead (BusAddress address, u16 value)
// {
// }

StatusCode KT24::writePhysical (u32 physicalAddress, u16 value)
{
    if (bus_->writeWord (physicalAddress, value))
        return StatusCode::OK;

    return StatusCode::NonExistingMemory;
}

// StatusCode KT24::readPhysical (u32 physicalAddress, u16* destination)
// {}

u32 KT24::physicalAddressFrom18BitBusAddress (BusAddress busAddress)
{
    size_t registerIndex = 
        indexFrom18BitBusAddress (busAddress.registerAddress ());
    // std::cout << "address.registerAddress(): " << address.registerAddress () << '\n';
    // std::cout << "registerIndex: " << registerIndex << '\n';

    u16 low = mappingRegisters_[registerIndex].low;
    u16 high = mappingRegisters_[registerIndex].high;

    // std::cout << "low: " << low << '\n';
    // std::cout << "high: " << high << '\n';

    return ((high << 16) | low) + (static_cast<u16> (busAddress) & 017777);
}
