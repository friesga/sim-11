#include "qbus.h"
#include "trace/trace.h"

bool Qbus::writeByte (u16 address, u8 value)
{
	PDP11Peripheral *module;
		
	if ((module = responsibleModule(address)) != nullptr)
	{
		trace.bus (BusRecordType::Write, address, value);
		module->writeByte (address, value);
		return true;
	}

	// No responsible module found
	return false;
}