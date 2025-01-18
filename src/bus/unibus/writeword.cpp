#include "unibus.h"
#include "trace/trace.h"

bool Unibus::writeWord (BusAddress address, u16 value)
{
	return addressMustBeMapped (address) ?
		mappedWriteWord (address, value) : physicalWriteWord (address, value);
}

bool Unibus::mappedWriteWord (BusAddress address, u16 value)
{
    return physicalWriteWord (unibusMap_->physicalAddressFrom18bitAddress (address), value);
}

bool Unibus::physicalWriteWord (BusAddress address, u16 value)
{
	BusDevice* module;

	// Prevent write's to odd addresses
	address &= 0xFFFFFFFE;

	if ((module = configurationHandler_.responsibleModule (address)) != nullptr)
	{
		trace.bus (BusRecordType::Write, address, value);
		module->writeWord (address, value);
		return true;
	}

	// No responsible module found
	return false;
}
