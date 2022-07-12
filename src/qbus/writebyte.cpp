#include "qbus.h"
#include "trace.h"

bool QBUS::writeByte (u16 address, u8 value)
{
	QBUSModule* module;
		
	if ((module = responsibleModule(address)) != nullptr)
	{
		TRCBus (TRC_BUS_WR, address, value);
		module->writeByte (address, value);
		return true;
	}

	// No responsible module found
	TRCBus (TRC_BUS_WRFAIL, address, value);
	setTrap (TrapPriority::BusError, 004);
	return false;
}