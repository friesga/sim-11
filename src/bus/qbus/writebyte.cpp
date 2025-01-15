#include "qbus.h"
#include "trace/trace.h"

bool Qbus::writeByte (BusAddress address, u8 value)
{
	BusDevice *module;
		
	if ((module = responsibleModule (address)) != nullptr)
	{
		trace.bus (BusRecordType::Write, address, value);
		module->writeByte (address, value);
		return true;
	}

	// No responsible module found
	return false;
}