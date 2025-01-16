#include "unibus.h"
#include "trace/trace.h"

bool Unibus::writeByte (BusAddress address, u8 value)
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

