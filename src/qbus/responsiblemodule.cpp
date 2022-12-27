#include "qbus.h"
#include "trace/trace.h"

std::shared_ptr<BusDevice> QBUS::responsibleModule (u16 address)
{
	for (size_t index = 0; index < LSI11_SIZE; ++index)
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