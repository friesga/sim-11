#include "qbus.h"
#include "trace/trace.h"

CondData<u16> Qbus::read (BusAddress address)
{
	BusDevice* module;

	// Prevent read's from odd addresses
	address &= 0xFFFFFFFE;

	if ((module = configurationHandler_.responsibleModule (address)) != nullptr)
	{
		u16 value;
		if (module->read (address, &value) == StatusCode::OK)
		{
			trace.bus (BusRecordType::Read, address, value);
			return value;
		}
		else
			return {};
	}

	return {};
}