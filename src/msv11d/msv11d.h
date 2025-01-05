#ifndef _MSV11D_H_
#define _MSV11D_H_

#include "qbus/qbus.h"
#include "memorydevice.h"
#include "configdata/msv11config/msv11config.h"

using std::shared_ptr;

class MSV11D : public MemoryDevice
{
public:
	MSV11D (Qbus* bus);
	MSV11D (Qbus* bus, shared_ptr<MSV11Config> msv11Config);
	~MSV11D ();

	// Functions required for the BusDevice interface
	StatusCode read (BusAddress<> address, u16 *destAddress) override;
	StatusCode writeWord (BusAddress<> address, u16 value) override;
	bool responsible (BusAddress<> address) override;
	void reset ();
	
	// Function required for the MemoryDevice interface
	u16 loadFile (const char* fileName);

	// Declare the signal receivers
	void BPOKReceiver (bool signalValue);

private:
	// Memory size is 64kB
	static const size_t MSV11D_SIZE {65536};
	u8* data;

	Qbus* bus_;
	MSV11Config::PowerSource powerSource_;
	u32 startingAddress_;
	bool bank7Lower2kWEnabled_;
};

#endif // _MSV11D_H_