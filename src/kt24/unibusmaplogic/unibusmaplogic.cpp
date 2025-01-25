#include "unibusmaplogic.h"
#include "trace/trace.h"

#include <iostream>
#include <sstream>
#include "chrono/simulatorclock/simulatorclock.h"

using std::bind;
using std::placeholders::_1;

UnibusMapLogic::UnibusMapLogic (Bus* bus)
    :
    bus_ {bus}
{
    bus_->IOMapEnable ().subscribe (bind (&UnibusMapLogic::ioMapEnableReceiver, this, _1));
}

UnibusMapLogic::UnibusMapLogic (Bus* bus, shared_ptr<KT24Config> kt24Config)
    :
    UnibusMapLogic (bus)
{}

void UnibusMapLogic::enable ()
{
    ioMapEnabled_ = true;
}

void UnibusMapLogic::disable ()
{
    ioMapEnabled_ = false;
}

StatusCode UnibusMapLogic::read (BusAddress address, u16* destination)
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

StatusCode UnibusMapLogic::writeWord (BusAddress address, u16 value)
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

StatusCode UnibusMapLogic::writeByte (BusAddress address, u8 value)
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

// The UnibusMapLogic is responsible for the given address if it is an address in one
// the defined kt24registers_ intervals.
bool UnibusMapLogic::responsible (BusAddress address)
{
    return findRegister (address) != UnibusMapLogic::End;
}

UnibusMapLogic::UBMLRegisters UnibusMapLogic::findRegister (BusAddress address) const
{
    if (address.isInIOpage ())
    {
        for (size_t index = 0; index < UnibusMapLogic::End; ++index)
        {
            if (kt24Registers_[index].contains (address.registerAddress ()))
                return static_cast<UBMLRegisters> (index);
        }
    }

    return UnibusMapLogic::End;
}

void UnibusMapLogic::readMappingRegister (u16 registerAddress, u16* destination)
{
    if (isLowRegister (registerAddress))
        *destination =
            mappingRegisters_[indexFromRegisterAddress (registerAddress)].low;
    else 
        *destination =
            mappingRegisters_[indexFromRegisterAddress (registerAddress)].high;
}

void UnibusMapLogic::readLMARegister (u16 registerAddress, u16* destination)
{
    if (isLowRegister (registerAddress))
        *destination = lmaRegister_.low;
    else
        *destination = lmaRegister_.high;
}

// The mapping registers are 21-bits wide and are stored in two 16-bit words,
// i.e. a low and a high word. Bit 0 of the low word is not writable and reads
// as zero. This is confirmed by diagnostic KKUAE0 test 2.
void UnibusMapLogic::writeMappingRegister (u16 registerAddress, u16 value)
{
    if (isLowRegister (registerAddress))
        mappingRegisters_[indexFromRegisterAddress (registerAddress)].low = 
            value & 0177776;
    else 
        mappingRegisters_[indexFromRegisterAddress (registerAddress)].high = value;
}

void UnibusMapLogic::writeLMARegister (u16 registerAddress, u16 value)
{
    if (isLowRegister (registerAddress))
        lmaRegister_.low = value;
    else
        lmaRegister_.high = value;
}

// The CPU error register is located at address 017 777 766. This register is
// read-only; any attempt to write to it will clear the power-fail bit.
void UnibusMapLogic::writeCpuErrorRegister ()
{
    cpuErrorRegister_ &= 0177776;
}

// A reset clears all mapping registers
void UnibusMapLogic::reset ()
{
    for (auto& mappingRegister : mappingRegisters_)
        mappingRegister = 0;
}

// The LMA also contains the memory control lines C1 and C0. According to
// Table 4-2 in the 11/24 Technical Manual both these bits are set on DATO
// (write) and DATOB (write byte) cycles. Diagnostic KKUAE0 test 26 and test
// 27 however check that just LMA bit 15 (C1) is set on a DATO cycle and both
// bit 14 (C0) and 15 (C1) are set on a DATOB cycle.
//
// In test 27 it is noted that the DATIP (read-modify-write) cycle cannot be
// checked because the LMA is written twice when a DATIP is executed,
// destroying the DATIP state that was written first.
//
void UnibusMapLogic::setControlLines (u16 controlLines)
{
    lmaRegister_.high = lmaRegister_.high & ~(UnibusMap::C0 | UnibusMap::C1) |
        (controlLines & (UnibusMap::C0 | UnibusMap::C1));
}

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
// Diagnostic KKUAE0 test 5 confirms that 22-bit addresses with the highest
// four bits set and with Unibus Map disabled have to be transformed to their
// 18-bit equivalent (i.e. the highest for bits have to be masked off).
//
BusAddress UnibusMapLogic::physicalAddressFrom18bitAddress (BusAddress address)
{
    if (!ioMapEnabled_)
        return BusAddress (static_cast<u32> (address) & 0777777,
            BusAddress::Width::_18Bit);

    if (!address.isInIOpage ())
    {
        u32 physicalAddress = _22bitAddressFrom18BitBusAddress (address);
        lmaRegister_.low = physicalAddress & 0177777;
        lmaRegister_.high = (physicalAddress >> 16) & 077;
        return BusAddress (physicalAddress, BusAddress::Width::_22Bit);
    }
    else
        // Return the given address as an 18-bit address. As the address is
        // in the I/O page, it does not matter if it's an 18- or 22-bit
        // address.
        return address;

}

u32 UnibusMapLogic::_22bitAddressFrom18BitBusAddress (BusAddress busAddress)
{
    size_t registerIndex =
        indexFrom18BitBusAddress (static_cast<u32> (busAddress));

    u16 low = mappingRegisters_[registerIndex].low;
    u16 high = mappingRegisters_[registerIndex].high;

    u32 physicalAddress = ((high << 16) | low) +
        (static_cast<u16> (busAddress) & 017777);

    trace.unibusMap (static_cast<u32> (busAddress), busAddress.width (),
        registerIndex, low, high, physicalAddress);

    return physicalAddress;
}

void UnibusMapLogic::ioMapEnableReceiver (bool signalValue)
{
    ioMapEnabled_ = signalValue;
}
