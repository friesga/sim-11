#include "qbus.h"
#include "trace/trace.h"

bool Qbus::writeWord (BusAddress address, u16 value)
{
	BusDevice *module;
	
	// Prevent write's to odd addresses
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