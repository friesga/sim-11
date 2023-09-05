#ifndef _BUSDEVICE_H_
#define _BUSDEVICE_H_

// Definition of the functions every Qbus module must provide.
//
// The following information structure is used:
//
//			    BusDevice
//		           ^
//                 |
//        +--------+---------+
//        |                  |
//   PDP11Processor  PDP11Peripheral
//
// BusDevice and PDP11Processor are interfaces, PDP11Peripheral is a
// base class for the peripheral devices.
//
class BusDevice
{
public:
	virtual StatusCode read (u16 addr, u16 *destination) = 0;
	virtual StatusCode writeWord (u16 addr, u16 value) = 0;
	virtual StatusCode writeByte (u16 addr, u8 value) = 0;
	virtual bool responsible (u16 addr) = 0;
	virtual void reset () = 0;
};

#endif // !_BUSDEVICE_H_
