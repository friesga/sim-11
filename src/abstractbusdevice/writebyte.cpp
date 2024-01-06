#include "abstractbusdevice.h"

// The defualt implementation of writeByte() for PDP11Peripheral's and
// PDP11Processor's. 
StatusCode AbstractBusDevice::writeByte (BusAddress address, u8 value)
{
    u16 wordAddress = address & 0xFFFE;
	u16 tmp;
	read (wordAddress, &tmp);
	if (address & 1)
		tmp = (tmp & 0x00FF) | (value << 8);
	else
		tmp = (tmp & 0xFF00) | value;

	return writeWord (wordAddress, tmp); 
}
