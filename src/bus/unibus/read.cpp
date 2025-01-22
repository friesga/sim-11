#include "unibus.h"
#include "trace/trace.h"

// 18-bit addresses and 22-bit addresses with the four highest bit set
// are mapped via the Unibus Map (if present). All other addresses are
// physical and can be routed directly to the device.
CondData<u16> Unibus::read (BusAddress address)
{
	return addressMustBeMapped (address) ? 
		mappedRead (address) : physicalRead (address);
}

CondData<u16> Unibus::mappedRead (BusAddress address)
{
	unibusMap_->setControlLines (0);
	return physicalRead (unibusMap_->physicalAddressFrom18bitAddress (address));
}

// On the Unibus and QBus reads are always word-sized (and from an even
// address) and writes may be either word or byte sized (with byte-sized
// writes using the upper or lower byte depending on whether the address
// is odd or even). When the CPU needs a byte it reads a word and ignores
// the portion of the word it isn't interested in.
// 
// Source: https://retrocomputing.stackexchange.com/questions/13262/how-movb-tstb-and-all-byte-instructions-works-in-odd-address-read-in-pdp11
//
CondData<u16> Unibus::physicalRead (BusAddress address)
{
	BusDevice* module;

	// Prevent read's from odd addresses
	address &= 0xFFFFFFFE;

	if ((module = configurationHandler_.responsibleModule (address)) != nullptr)
	{
		u16 value;
		if (module->read (address, &value) == StatusCode::OK)
		{
			trace.bus (BusRecordType::Read, address, value);
			return value;
		}
		else
			return {};
	}

	return {};
}
