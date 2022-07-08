#include "qbus.h"

bool QBUS::write8 (u16 addr, u8 val)
{
    u16 aaddr = addr & 0xFFFE;
	u16 tmp = read (aaddr);
	if(addr & 1)
		tmp = (tmp & 0x00FF) | (val << 8);
	else
		tmp = (tmp & 0xFF00) | val;
	return writeWord (aaddr, tmp); 
}