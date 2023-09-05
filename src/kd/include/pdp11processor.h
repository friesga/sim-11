#ifndef _PDP11PROCESSOR_H_
#define _PDP11PROCESSOR_H_

#include "types.h"
#include "pdp11peripheral/pdp11peripheral.h"


// This interface defines the functions a PDP-11 processor has to implement
// to be able to configure and run it.
//
// Every PDP-11 processor is a bus device and therefor has to implement the
// BusDevice interface.
class PDP11Processor : public BusDevice
{
public:
	virtual void start (u16 startAddress) = 0;
	virtual void start () = 0;
};

#endif // !_PDP11PROCESSOR_H_
