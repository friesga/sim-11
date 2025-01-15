#include "configurationhandler.h"


BusDevice* ConfigurationHandler::responsibleModule (BusAddress address)
{
	for (size_t index = 0; index < numberOfSlots; ++index)
	{
		auto module = slots_[index];
		if (!module)
			continue;

		if (module->responsible (address))
			return module;
	}

	// No responsible module found
	return nullptr;
}