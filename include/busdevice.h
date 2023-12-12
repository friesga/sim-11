#ifndef _BUSDEVICE_H_
#define _BUSDEVICE_H_

#include "types.h"
#include "statuscodes.h"

// Definition of the functions every Qbus module must provide.
//
// The following information structure is used:
//
//			    BusDevice
//		           ^
//                 |
//			AbstractBusDevice
//				   |
//        +--------+---------+
//        |                  |
//   PDP11Processor  PDP11Peripheral
//
// BusDevice is the interface for all PDP11Processor's and PDP11Peripheral's.
// AbstractBusDevice is a partial implementation of the BusDevice interface,
// providing a writeByte() function. PDP11Processor is an interface for
// processor implementation and PDP11Peripheral is a base class for the
// peripheral devices.
//
class BusDevice
{
public:
	virtual StatusCode read (u16 address, u16 *destination) = 0;
	virtual StatusCode writeWord (u16 address, u16 value) = 0;
	virtual StatusCode writeByte (u16 address, u8 value) = 0;
	virtual bool responsible (u16 address) = 0;
	virtual void reset () = 0;
};

#endif // !_BUSDEVICE_H_
