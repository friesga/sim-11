#include "qbus.h"
#include "trace/trace.h"

template <ValidBusAddress T>
bool Qbus::writeWord (T address, u16 value)
{
	BusDevice *module;
	
	// Prevent write's to odd addresses
	address &= 0xFFFFFFFE;

	if ((module = responsibleModule(address)) != nullptr)
	{
		trace.bus (BusRecordType::Write, address, value);
		module->writeWord (address, value);
		return true;
	}

	// No responsible module found
	return false;
}