#include "kt24.h"

#include <iostream>

KT24::KT24 (Bus* bus)
    :
    bus_ {bus}
{}

KT24::KT24 (Bus* bus, shared_ptr<KT24Config> kt24Config)
    :
    bus_ {bus}
{}

void KT24::enable ()
{
    ioMapEnabled_ = true;
}

void KT24::disable ()
{
    ioMapEnabled_ = false;
}

StatusCode KT24::read (BusAddress address, u16* destination)
{
    u16 registerAddress= address.registerAddress ();

    switch (findRegister (registerAddress))
    {
        case MappingRegister:
            readMappingRegister (registerAddress, destination);
            break;

        case LMARegister:
            readLMARegister (registerAddress, destination);
            break;

        case CpuErrorRegister:
            *destination = cpuErrorRegister_;
            break;

        default:
            throw "Should not happen";
    }

    return StatusCode::OK;
}

StatusCode KT24::writeWord (BusAddress address, u16 value)
{
    u16 registerAddress= address.registerAddress ();

    switch (findRegister (registerAddress))
    {
        case MappingRegister:
            writeMappingRegister (registerAddress, value);
            break;

        case LMARegister:
            writeLMARegister (registerAddress, value);
            break;

        case CpuErrorRegister:
            writeCpuErrorRegister ();
            break;

        default:
            throw "Should not happen";
    }

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

// The KT24 is responsible for the given address if it is an address in one
// the defined kt24registers_ intervals.
bool KT24::responsible (BusAddress address)
{
    return findRegister (address) != KT24Registers::End;
}

KT24::KT24Registers KT24::findRegister (BusAddress address) const
{
    if (address.isInIOpage ())
    {
        for (size_t index = 0; index < KT24Registers::End; ++index)
        {
            if (kt24Registers_[index].contains (address.registerAddress ()))
                return static_cast<KT24Registers> (index);
        }
    }

    return KT24Registers::End;
}

void KT24::readMappingRegister (u16 registerAddress, u16* destination)
{
    if (isLowRegister (registerAddress))
        *destination =
            mappingRegisters_[indexFromRegisterAddress (registerAddress)].low;
    else 
        *destination =
            mappingRegisters_[indexFromRegisterAddress (registerAddress)].high;
}

void KT24::readLMARegister (u16 registerAddress, u16* destination)
{
    if (isLowRegister (registerAddress))
        *destination = lmaRegister_.low;
    else
        *destination = lmaRegister_.high;
}

// The mapping registers are 21-bits wide and are stored in two 16-bit words,
// i.e. a low and a high word. Bit 0 of the low word is not writable and reads
// as zero. This is confirmed by diagnostic KKUAE0 test 2.
void KT24::writeMappingRegister (u16 registerAddress, u16 value)
{
    if (isLowRegister (registerAddress))
        mappingRegisters_[indexFromRegisterAddress (registerAddress)].low = 
            value & 0177776;
    else 
        mappingRegisters_[indexFromRegisterAddress (registerAddress)].high = value;
}

void KT24::writeLMARegister (u16 registerAddress, u16 value)
{
    if (isLowRegister (registerAddress))
        lmaRegister_.low = value;
    else
        lmaRegister_.high = value;
}

// The CPU error register is located at address 017 777 766. This register is
// read-only; any attempt to write to it will clear the power-fail bit.
void KT24::writeCpuErrorRegister ()
{
    cpuErrorRegister_ &= 0177776;
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
// It should be noted that the last mapping register (addresses 017770374 and
// 017770376) can be read or written, but cannot be used to map Unibus
// addresses because it would be used by addresses in the range of the
// I/O page (017760000 - 017777777).
//
CondData<u16> KT24::dmaRead (BusAddress address)
{
    return ioMapEnabled_ && !address.isInIOpage () ? 
        mappedRead (address) :
        readPhysical (address);
}

bool KT24::dmaWrite (BusAddress address, u16 value)
{
    return ioMapEnabled_  && !address.isInIOpage () ?
        mappedWrite (address, value) :
        writePhysical (address, value);
}

CondData<u16> KT24::mappedRead (BusAddress address)
{
    u32 physicalAddress = physicalAddressFrom18BitBusAddress (address);
    lmaRegister_ = physicalAddress;
    return readPhysical (physicalAddress);
}

bool KT24::mappedWrite (BusAddress address, u16 value)
{
    u32 physicalAddress = physicalAddressFrom18BitBusAddress (address);
    lmaRegister_ = physicalAddress;
    return writePhysical (physicalAddress, value);
}

CondData<u16> KT24::readPhysical (u32 physicalAddress)
{
    return bus_->read (physicalAddress);
}

bool KT24::writePhysical (u32 physicalAddress, u16 value)
{
    return bus_->writeWord (physicalAddress, value);
}

u32 KT24::physicalAddressFrom18BitBusAddress (BusAddress busAddress)
{
    size_t registerIndex = 
        indexFrom18BitBusAddress (busAddress.registerAddress ());
    // std::cout << "address.registerAddress(): " << busAddress.registerAddress () << '\n';
    // std::cout << "registerIndex: " << registerIndex << '\n';

    u16 low = mappingRegisters_[registerIndex].low;
    u16 high = mappingRegisters_[registerIndex].high;

    // std::cout << "low: " << low << '\n';
    // std::cout << "high: " << high << '\n';

    //std::cout << "physicalAddress: " << 
    //    ((high << 16) | low) + (static_cast<u16> (busAddress) & 017777) << '\n';

    return ((high << 16) | low) + (static_cast<u16> (busAddress) & 017777);
}

void KT24::ioMapEnableReceiver (bool signalValue)
{
    ioMapEnabled_ = signalValue;
}
