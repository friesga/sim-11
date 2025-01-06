#ifndef _BUS_H_
#define _BUS_H_

#include "types.h"
#include "conddata/conddata.h"
#include "interruptrequest/interruptrequest.h"
#include "signal/signal.h"
#include "busaddress/busaddress.h"
#include "busdevice.h"

class Bus
{
public:
	virtual Signal& SRUN () = 0;
	virtual Signal& BPOK () = 0;
	virtual Signal& RESET () = 0;
	virtual Signal& BHALT () = 0;
	virtual Signal& BINIT () = 0;
	virtual Signal& BOOT () = 0;
	virtual Signal& BatteryPower () = 0;

	virtual void setInterrupt (TrapPriority priority, unsigned char busOrder,
		u8 functionOrder, unsigned char vector) = 0;
	virtual bool containsInterrupt (TrapPriority priority, unsigned char busOrder,
		u8 functionOrder) = 0;
	virtual void clearInterrupt (TrapPriority priority, unsigned char busOrder,
		u8 functionOrder) = 0;
	virtual void clearInterrupts () = 0;
	virtual bool intrptReqAvailable () = 0;
	virtual u8 intrptPriority () = 0;
	virtual bool getIntrptReq (InterruptRequest& ir) = 0;

	virtual CondData<u16> read (BusAddress address) = 0;
	virtual bool writeWord (BusAddress address, u16 value) = 0;
	virtual bool writeByte (BusAddress address, u8 val) = 0;
	virtual CondData<u16> dmaRead (BusAddress address) = 0;
	virtual bool dmaWrite (BusAddress address, u16 value) = 0;
	virtual bool installModuleAtPosition (BusDevice* module, size_t position) = 0;
	virtual bool installModule (BusDevice* module) = 0;
};

#endif // _BUS_H_