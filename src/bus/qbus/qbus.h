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
	Signal& SRUN () override;
	Signal& BPOK () override;
	Signal& RESET () override;
	Signal& BHALT () override;
	Signal& START () override;
	Signal& BINIT () override;
	Signal& BOOT () override;
	Signal& BatteryPower () override;
	Signal& IOMapEnable () override;

	// Functions required for the BusInterrupts interface
	void requestInterrupt (InterruptPriority priority, unsigned char busOrder,
		u8 functionOrder, u16 vector, function<void ()> requestGrant = 0) override;
	bool containsInterrupt (InterruptPriority priority, unsigned char busOrder,
		u8 functionOrder) override;
	void clearInterrupt (InterruptPriority priority, unsigned char busOrder,
		u8 functionOrder) override;
	void clearInterrupts () override;
	bool intrptReqAvailable () override;
	u8 intrptPriority () override;
	bool getIntrptReq (InterruptRequest& ir) override;

	// Functions required for the BusConfiguration interface
	bool installModuleAtPosition (BusDevice* module, size_t position) override;
	bool installModule (BusDevice* module) override;
	BusDevice* responsibleModule (BusAddress address) override;
	Iterator begin () override;
	Iterator end () override;
	size_t capacity () override;
	Iterator operator[] (int index) override;
	void reset () override;

	// Functions required for the BusDataTransfer interface
	CondData<u16> read (BusAddress address) override;
	bool writeWord (BusAddress address, u16 value) override;
	bool writeByte (BusAddress address, u8 val) override;

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