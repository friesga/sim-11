#include "qbus.h"
#include "trace/trace.h"

bool QBUS::writeByte (u16 address, u8 value)
{
	BusDevice* module;
		
	if ((module = responsibleModule(address)) != nullptr)
	{
		TRCBus (TRC_BUS_WR, address, value);
		module->writeByte (address, value);
		return true;
	}

	// No responsible module found
	return false;
}