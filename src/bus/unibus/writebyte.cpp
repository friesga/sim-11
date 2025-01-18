#include "unibus.h"
#include "trace/trace.h"

bool Unibus::writeByte (BusAddress address, u8 value)
{
	return addressMustBeMapped (address) ?
		mappedWriteByte (address, value) : physicalWriteByte (address, value);
}

bool Unibus::mappedWriteByte (BusAddress address, u8 value)
{
	return physicalWriteWord (unibusMap_->physicalAddressFrom18bitAddress (address), value);
}

bool Unibus::physicalWriteByte (BusAddress address, u8 value)
{
	BusDevice* module;

	if ((module = configurationHandler_.responsibleModule (address)) != nullptr)
	{
		trace.bus (BusRecordType::Write, address, value);
		module->writeByte (address, value);
		return true;
	}

	// No responsible module found
	return false;
}

