#include "qbus.h"
#include "trace/trace.h"

PDP11Peripheral *Qbus::responsibleModule (u16 address)
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