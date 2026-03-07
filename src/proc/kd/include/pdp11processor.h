#ifndef _PDP11PROCESSOR_H_
#define _PDP11PROCESSOR_H_

#include "types.h"
#include "abstractbusdevice/abstractbusdevice.h"


// This interface defines the functions a PDP-11 processor has to implement
// to be able to configure and run it.
//
// Every PDP-11 processor is a bus device and therefore has to implement the
// BusDevice interface. It uses the AbstractBusDevice class as that provides
// a writeByte(0 function.
//
class PDP11Processor : public AbstractBusDevice
{
public:
	virtual void start (u16 startAddress) = 0;
	virtual void start () = 0;

	// Definition of a (dummy) virtual destructor so the compiler is satified the
	// base class is destructed properly.
	virtual ~PDP11Processor () {};
};

#endif // !_PDP11PROCESSOR_H_
