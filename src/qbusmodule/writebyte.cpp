#include "qbusmodule.h"

// The defualt implementation of writeByte() for QBus modules. This
// implementation can be overridden if needed.
void QBUSModule::writeByte (u16 addr, u8 value)
{
    u16 aaddr = addr & 0xFFFE;
	u16 tmp;
	read (aaddr, &tmp);
	if(addr & 1)
		tmp = (tmp & 0x00FF) | (value << 8);
	else
		tmp = (tmp & 0xFF00) | value;

	writeWord (aaddr, tmp); 
}