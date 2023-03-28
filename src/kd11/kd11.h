#ifndef _KD11_H_
#define _KD11_H_

#include "cpu/kd11cpu.h"
#include "odt/kd11odt.h"
#include "configdata/kd11config/kd11config.h"

using namespace KD11_F;

#include <memory>

using std::unique_ptr;
using std::shared_ptr;

// The class KD11 is composed of the KD11 CPU and the KD11 ODT.
class KD11 : public BusDevice
{
public:
	KD11 (Qbus *bus);
	KD11 (Qbus *bus, shared_ptr<KD11Config> kd11Config);
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
	unique_ptr<KD11ODT>	odt_ {};
	KD11Config::PowerUpMode powerUpMode;
};

#endif // !_KD11_H_
