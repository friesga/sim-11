#ifndef _KD11_H_
#define _KD11_H_

#include "cpu/kd11cpu.h"
#include "odt//kd11odt.h"

// The class KD11 is composed of the KD11 CPU and the KD11 ODT.
// ToDo: Make KD11 a BusDevice
// The module KD11 is now - different from the other QBUSModules - instantiated
// within the LSI11 object, although it is also a QBUS module.
class KD11
{
public:
	KD11 (QBUS *bus);
	void reset();
	void step(QBUS* bus);
	// Give main() access to the CPU to set PC and runState
	KD11CPU &cpu();

private:
	QBUS *bus_;
	KD11CPU cpu_ {bus_};
	KD11ODT	odt {cpu_};
};

#endif // !_KD11_H_
