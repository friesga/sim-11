#ifndef _MSV11D_H_
#define _MSV11D_H_

#include "qbus/qbus.h"
#include "pdp11peripheral/pdp11peripheral.h"
#include "configdata/msv11config/msv11config.h"

using std::shared_ptr;

/* Main memory size: 32kW / 64kB */
#define	MSV11D_SIZE		65536

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
	MSV11Config::PowerSource powerSource_;
};

#endif // _MSV11D_H_