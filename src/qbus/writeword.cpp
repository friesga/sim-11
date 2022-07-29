#include "qbus.h"
#include "trace.h"

bool QBUS::writeWord (u16 address, u16 value)
{
	BusDevice* module;

	address &= 0xFFFE;
		
	if ((module = responsibleModule(address)) != nullptr)
	{
		TRCBus (TRC_BUS_WR, address, value);
		module->writeWord (address, value);
		return true;
	}

	// No responsible module found
	TRCBus (TRC_BUS_WRFAIL, address, value);
	setTrap (TrapPriority::BusError, 004);
	return false;
}