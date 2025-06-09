#ifndef _QBUS_H_
#define _QBUS_H_

#include "bus/include/bus.h"
#include "bus/signalhandler/signalhandler.h"
#include "bus/interrupthandler/interrupthandler.h"
#include "bus/configurationhandler/configurationhandler.h"
#include "bus/iterator/iterator.h"

#include <functional>

using std::function;

// This class implements the Qbus backplane. Two variants of the backplane
// exist: 18-bit and 22-bit wide. As the 18-bit backplanes can be upgraded
// to a 22-bit without impact on the functionality of the system we implement
// the 22-bit version.
//
// Addresses on the Qbus are represented by a 32-bit integer. Devices in the
// I/O page ignore the nine high-order address bits BDAL<21:13> and instead
// decode BBS7 [which indicates an I/O page access] along with the thirteen
// low-order address bits. (Supersystems Handbook)
// 
// This means that register address for devices with registers in the I/O
// page can be limited to 16-bits. The 32-bit bus address will be truncated
// to a 16 bit address to be used by the device.
//
class Qbus : public Bus
{
public:
	Qbus ();

	// Functions required for the BusSignals interface
	Signal& SRUN ();
	Signal& BPOK ();
	Signal& RESET ();
	Signal& BHALT ();
	Signal& BINIT ();
	Signal& BOOT ();
	Signal& BatteryPower ();
	Signal& IOMapEnable ();

	// Functions required for the BusInterrupts interface
	void busRequest (TrapPriority priority, unsigned char busOrder,
		u8 functionOrder, function<void ()> busGrant = 0);
	void setInterrupt (TrapPriority priority, unsigned char busOrder,
		u8 functionOrder, unsigned char vector, function<void ()> ack = 0);
	bool containsInterrupt (TrapPriority priority, unsigned char busOrder,
		u8 functionOrder);
	void clearInterrupt (TrapPriority priority, unsigned char busOrder,
		u8 functionOrder);
	void clearInterrupts ();
	bool intrptReqAvailable ();
	u8 intrptPriority ();
	bool getIntrptReq (InterruptRequest& ir);

	// Functions required for the BusConfiguration interface
	bool installModuleAtPosition (BusDevice* module, size_t position);
	bool installModule (BusDevice* module);
	BusDevice* responsibleModule (BusAddress address);
	Iterator begin ();
	Iterator end ();
	size_t capacity ();
	Iterator operator[] (int index);
	void reset ();

	// Functions required for the BusDataTransfer interface
	CondData<u16> read (BusAddress address);
	bool writeWord (BusAddress address, u16 value);
	bool writeByte (BusAddress address, u8 val);

private:
	// Definition of the handlers
	SignalHandler signalHandler_;
	InterruptHandler interruptHandler_;
	ConfigurationHandler configurationHandler_;

	// Signal administration
	Signal::SubscriberKey ourKey_;

	void BINITReceiver (bool signalValue);
};


#endif // !_QBUS_H_