#ifndef _MSV11D_H_
#define _MSV11D_H_

#include "qbus/qbus.h"
#include "pdp11peripheral/pdp11peripheral.h"
#include "configdata/msv11config/msv11config.h"

using std::shared_ptr;

class MSV11D : public PDP11Peripheral
{
public:
	MSV11D (Qbus* bus);
	MSV11D (Qbus* bus, shared_ptr<MSV11Config> msv11Config);
	~MSV11D ();
	StatusCode read (BusAddress address, u16 *destAddress) override;
	StatusCode writeWord (BusAddress address, u16 value) override;
	bool responsible (BusAddress address) override;
	void reset ();
	
	// Declare the signal receivers
	void BPOKReceiver (bool signalValue);

	// ToDo: Make data private (accessed from LSI11::loadFile())
	u8*	data;

private:
	// Memory size is 64kB
	static const size_t MSV11D_SIZE {65536};

	MSV11Config::PowerSource powerSource_;
	u32 startingAddress_;
	bool bank7Lower2kWEnabled_;
};

#endif // _MSV11D_H_