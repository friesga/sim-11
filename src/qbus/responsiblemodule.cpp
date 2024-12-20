#include "qbus.h"
#include "trace/trace.h"

BusDevice *Qbus::responsibleModule (BusAddress address)
{
	for (size_t index = 0; index < numberOfSlots; ++index)
	{
		auto module = slots[index];
		if (!module)
			continue;

		if (module->responsible (address))
			return module;
	}

	// No responsible module found
	return nullptr;
}