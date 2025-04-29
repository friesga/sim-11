#ifndef _BUSDEVICE_H_
#define _BUSDEVICE_H_

#include "types.h"
#include "statuscodes.h"
#include "busaddress/busaddress.h"
#include "conddata/conddata.h"

// Definition of the functions every Qbus module must provide.
//
// The following information structure is used:
//
//			             BusDevice
//		                     ^
//                           |
//         			AbstractBusDevice
//				             |
//        +------------------+-------------+
//        |                                |
//   PDP11Processor                  MemoryDevice
//
// BusDevice is the interface for all PDP11Processor's and peripherals.
// AbstractBusDevice is a partial implementation of the BusDevice interface,
// providing a writeByte() function. PDP11Processor is an interface for
// processor implementations and MemoryDevice is interface for memory modules.
//
class BusDevice
{
public:
	virtual CondData<u16> read (BusAddress address) = 0;
	virtual StatusCode writeWord (BusAddress address, u16 value) = 0;
	virtual StatusCode writeByte (BusAddress address, u8 value) = 0;
	virtual bool responsible (BusAddress address) = 0;
	virtual void reset () = 0;
	virtual ~BusDevice() {};
};

#endif // !_BUSDEVICE_H_
