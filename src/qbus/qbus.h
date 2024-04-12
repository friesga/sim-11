#ifndef _QBUS_H_
#define _QBUS_H_

#include "types.h"
#include "statuscodes.h"
#include "conddata/conddata.h"
#include "interruptrequest/interruptrequest.h"
#include "threadsafecontainers/threadsafeprioqueue.h"
#include "pdp11peripheral/pdp11peripheral.h"
#include "signal/signal.h"

#include <string>
#include <array>
#include <functional>
#include <vector>

using std::string;
using std::array;
using std::function;
using std::vector;

// A BA11-N backplane has nine slots, numbered 1 to 9.
#define	LSI11_SIZE		9

// Qbus interrupt latency, defined as the maximum number of instructions
// after which the interrupt will be processed. The INTRPT_LATENCY_JITTER
// is a random number of instructions. The minimum latency will be
// INTRPT_LATENCY - INTRPT_LATENCY_JITTER and the maximum latency is the
// value of INTRPT_LATENCY.
// 
// According to the Microcomputer Processor Handbook, the interrupt
// latency lies between 35.05 and 44.1 +/- 20%, so roughly between 30 and
// 50 microseconds. The LSI-11 instruction time varies between 3.5
// and 15 microseconds.
// 
// At least a few instructions are necessary to satify the VKAAC0 and 
// VKADC1 diagnostics. An interrupt latency of 20 instructions will result
// in the VRLBC0 diagnostic reporting "NO INTERRUPT ON FUNCTION COMPLETE"
// errors. The XRLKB3 diagnostic expects an interrupt within five instructions
// after Controller Ready becomes true.
#define	INTRPT_LATENCY			5
#define	INTRPT_LATENCY_JITTER	2

class PDP11Peripheral;

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
class Qbus
{
public:
	// The Qbus contains a set of control signals that can be set, cycled
	// and tested.
	//
	// The SRUN L signal is actually not a bus signal, but a non-bused, backplane
	// signal. The signal is a series of pulses which occur at 3-5" intervals
	// whenever the processor is in the Run mode.
	//
	// The BPOK signal is triggered:
	// - On power up or,
	// - When the console BREAK key is hit and the boot response is configured.
	// On assertion of this signal the processor executes the power up routine.
	// 
	// The PDP Bus Handbook states that "BOCOK H may be pulsed low for a minimum
	// of 1 microsecond to cause the CPU to restart" while BPOK is high. This
	// pulse of BDCOK is replaced by a separate RESET signal.
	// 
	// The BINIT signal is triggered:
	// - When the processor executes the power up routine,
	// - On a RESET instruction,
	// - On a ODT Go command.
	// On assertion of this signal all bus devices - including the bus itself -
	// are initialized.
	// 
	// The Qbus defines a number of (not bused) spare signals. Analoguously
	// we define an exit signal indicating the simulator has to exit.
	//
	Signal &SRUN ();
	Signal &BPOK ();
	Signal &RESET ();
	Signal &BHALT ();
	Signal &BINIT ();

	Qbus ();
	void setInterrupt (TrapPriority priority, unsigned char busOrder,
		u8 functionOrder, unsigned char vector);
	bool containsInterrupt (TrapPriority priority, unsigned char busOrder,
		u8 functionOrder);
	void clearInterrupt (TrapPriority priority, unsigned char busOrder,
		u8 functionOrder);
	void clearInterrupts ();
	bool intrptReqAvailable ();
	u8 intrptPriority ();
	bool getIntrptReq (InterruptRequest &ir);

	void step ();
	CondData<u16> read (BusAddress address);
	bool writeWord (BusAddress address, u16 value);
	bool writeByte (BusAddress address, u8 val);
	void installModule (int slot, BusDevice *module);

private:
	BusDevice *slots[LSI11_SIZE] {nullptr};

	// This queue keeps all interrupt requests, ordered in interrupt priority
	using IntrptReqQueue = ThreadSafePrioQueue<InterruptRequest>;
	IntrptReqQueue intrptReqQueue_;

	// Definition of the bus signals
	Signal SRUN_;
	Signal BPOK_;
	Signal RESET_;
	Signal BHALT_;
	Signal BINIT_;

	// Signal administration
	Signal::SubscriberKey ourKey_;

	bool processorRunning_;
	u16	delay_;

	void reset ();
	BusDevice *responsibleModule (BusAddress address);
	void pushInterruptRequest (InterruptRequest interruptReq);
	void BINITReceiver (bool signalValue);
};


#endif // !_QBUS_H_