#ifndef _BUSINTERFACE_H_
#define _BUSINTERFACE_H_

#include "bus/include/bus.h"
#include "busaddress/busaddress.h"
#include "conddata/conddata.h"
#include "types.h"

// This class provides an interface from the MMU to the bus. It is used by
// the MMU to read and write data on the bus, using physical addresses.
//
class BusInterface
{
public:
	BusInterface (Bus* bus);
	CondData<u16> read (BusAddress address);
	bool writeWord (BusAddress address, u16 value);
	bool writeByte (BusAddress address, u8 value);

private:
	Bus* bus_;
};

#endif // _BUSINTERFACE_H_