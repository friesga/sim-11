#include "abstractbusdevice.h"

// The default implementation of writeByte() for PDP11Peripheral's and
// PDP11Processor's. 
StatusCode AbstractBusDevice::writeByte (BusAddress address, u8 value)
{
	BusAddress wordAddress = address & 0xFFFFFFFE;
	u16 tmp = read (wordAddress);
	if (address & static_cast<u32> (1))
		tmp = (tmp & 0x00FF) | (value << 8);
	else
		tmp = (tmp & 0xFF00) | value;

	return writeWord (wordAddress, tmp); 
}
