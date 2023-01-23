#include "qbus.h"
#include "trace/trace.h"

bool QBUS::writeWord (u16 address, u16 value)
{
	std::shared_ptr<BusDevice> module;

	address &= 0xFFFE;
		
	if ((module = responsibleModule(address)) != nullptr)
	{
		trc.bus (BusRecordType::Write, address, value);
		module->writeWord (address, value);
		return true;
	}

	// No responsible module found
	return false;
}