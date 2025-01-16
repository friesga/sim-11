#include "unibus.h"
#include "trace/trace.h"

bool Unibus::writeWord (BusAddress address, u16 value)
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

bool Unibus::dmaWrite (BusAddress address, u16 value)
{
	return writeWord (address, value);
}