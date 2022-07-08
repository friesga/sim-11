#include "qbus.h"
#include "trace.h"

bool QBUS::writeWord (u16 address, u16 value)
{
	QBUSModule* module;

	address &= 0xFFFE;
		
	if ((module = responsibleModule(address)) != nullptr)
	{
		TRCBus (TRC_BUS_WR, address, value);
		module->writeWord (address, value);
		return true;
	}

	// No responsible module found
	TRCBus (TRC_BUS_WRFAIL, address, value);
	interrupt (busError);
	return false;
}