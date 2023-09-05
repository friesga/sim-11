#include "qbus.h"
#include "trace/trace.h"

bool Qbus::writeWord (u16 address, u16 value)
{
	PDP11Peripheral *module;

	address &= 0xFFFE;
		
	if ((module = responsibleModule(address)) != nullptr)
	{
		trace.bus (BusRecordType::Write, address, value);
		module->writeWord (address, value);
		return true;
	}

	// No responsible module found
	return false;
}