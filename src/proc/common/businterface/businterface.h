#ifndef _BUSINTERFACE_H_
#define _BUSINTERFACE_H_

#include "bus/include/bus.h"
#include "busaddress/busaddress.h"
#include "conddata/conddata.h"
#include "types.h"
#include "proc/include/ky11console.h"

// This class provides an interface from the MMU to the bus. It is used by
// the MMU to read and write data on the bus, using physical addresses.
//
class BusInterface
{
public:
	BusInterface (Bus* bus);

    // The console pointer has to be attached separately instead of being
	// passed in the constructor as there exists a circular dependency
	// BusInterFace -> KY11_A -> CompositeCpuController -> PseudoMMU ->
	// BusInterface.
    // ToDo: Remove the circular dependency
	//
	void attachConsole (KY11Console* console);

	CondData<u16> read (BusAddress address);
	bool writeWord (BusAddress address, u16 value);
	bool writeByte (BusAddress address, u8 value);

private:
	Bus* bus_;
	KY11Console* console_ {nullptr};
};

#endif // _BUSINTERFACE_H_