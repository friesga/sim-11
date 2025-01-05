#include "qbus.h"
#include "trace/trace.h"

template <ValidBusAddress T>
bool Qbus::writeByte (T address, u8 value)
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