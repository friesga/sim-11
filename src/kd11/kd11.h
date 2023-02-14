#ifndef _KD11_H_
#define _KD11_H_

#include "cpu/kd11cpu.h"
#include "odt//kd11odt.h"

// The class KD11 is composed of the KD11 CPU and the KD11 ODT.
class KD11 : public BusDevice
{
public:
	KD11 (Qbus *bus);
	void step ();
	
	// Give main() access to the CPU to set PC and runState
	KD11CPU &cpu();

	// The KD11 is a BusDevice without registers so the read and write 
	// register functions are dummies. The reset functie is called on a
	// bus reset and has no function for the KD11 either.
	StatusCode read (u16 addr, u16 *destination) override 
		{ return StatusCode::FunctionNotImplemented; };
	StatusCode writeWord (u16 addr, u16 value) override
		{ return StatusCode::FunctionNotImplemented; };
	StatusCode writeByte (u16 addr, u8 value) override
		{ return StatusCode::FunctionNotImplemented; };
	bool responsible (u16 addr) override { return false; }
	void reset() override {};

private:
	KD11CPU cpu_ {bus_};
	KD11ODT	odt_ {bus_, cpu_};
};

#endif // !_KD11_H_
