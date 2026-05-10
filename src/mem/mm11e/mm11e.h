#ifndef _MM11E_H_
#define _MMM11_H_

#include "bus/include/bus.h"
#include "memorydevice.h"
#include "configdata/mm11_econfig/mm11_econfig.h"

class MM11E : public MemoryDevice
{
public:
	MM11E (Bus* bus);
	MM11E (Bus* bus, const MM11EConfig& mm11eConfig);
	~MM11E ();

	// Functions required for the BusDevice interface
	CondData<u16> read (BusAddress address) override;
	StatusCode writeWord (BusAddress address, u16 value) override;
	bool responsible (BusAddress address) override;
	void reset () override;
	
	// Function required for the MemoryDevice interface
	u16 loadFile (const char* fileName) override;

private:
	// Memory size is 8KB
	static const size_t MM11E_SIZE {8192};
	u8* data;

	Bus* bus_;
	u32 startingAddress_;
};

#endif // _MM11E_H_