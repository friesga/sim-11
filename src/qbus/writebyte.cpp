#include "qbus.h"
#include "trace.h"

// ToDo: Remove code duplication between QBUS::writeWord() and writeByte()
bool QBUS::writeByte (u16 address, u8 value)
{
	/*
    u16 aaddr = addr & 0xFFFE;
	u16 tmp = read (aaddr);
	if(addr & 1)
		tmp = (tmp & 0x00FF) | (val << 8);
	else
		tmp = (tmp & 0xFF00) | val;
	return writeWord (aaddr, tmp); 
	*/

	u8 i;
	for (i = 0; i < LSI11_SIZE; i++)
	{
		QBUSModule* module = slots[i];
		if (!module)
			continue;

		if (module->responsible (address))
		{
			// It is presumed that all writes on addresses for which a
			// device is responsible succeed.
			TRCBus (TRC_BUS_WR, address, value);
			module->writeByte (address, value);
			return true;
		}
	}

	TRCBus (TRC_BUS_WRFAIL, address, value);
	interrupt (busError);
	return false;
}