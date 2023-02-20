#ifndef _QBUS_H_
#define _QBUS_H_

#include "types.h"
#include "statuscodes.h"
#include "conddata/conddata.h"
#include "interruptrequest/interruptrequest.h"
#include "threadsafecontainers/threadsafeprioqueue.h"
#include "busdevice/busdevice.h"

#include <string>
#include <array>

using std::string;
using std::array;

/* Backplane size */
#define	LSI11_SIZE		8

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

class BusDevice;

class Qbus
{
public:
	// The enumeration Signal defines a set of Qbus control signals that can
	// be given the value of the enumeration SignalValue.
	//
	// The SRUN L signal is actually not a bus signal, but a non-bused, backplane
	// signal. The signal is a series of pulses which occur at 3-5" intervals
	// whenever the processor is in the Run mode.
	enum class Signal
    {
        SRUN,
        BDCOK,
        BHALT,
        Count
    };

	enum class SignalValue
	{
		False,
		True,
		Cycle
	};

	Qbus ();
	void setInterrupt (TrapPriority priority, 
		unsigned char busOrder, unsigned char vector);
	void clearInterrupt (TrapPriority priority, unsigned char busOrder);
	void clearInterrupts ();
	bool intrptReqAvailable ();
	u8 intrptPriority ();
	bool getIntrptReq (InterruptRequest &ir);

	void setSignal (Signal signal, SignalValue value);
	bool signalIsSet (Signal signal);
	
	void reset ();
	void step ();
	CondData<u16> read (u16 addr);
	bool writeWord (u16 addr, u16 value);
	bool writeByte (u16 addr, u8 val);
	void installModule (int slot, BusDevice *module);
	BusDevice *findModuleByName (string moduleName);

private:
	BusDevice *slots[LSI11_SIZE] {nullptr};

	// This queue keeps all interrupt requests, ordered in interrupt priority
	using IntrptReqQueue = ThreadSafePrioQueue<InterruptRequest>;
	IntrptReqQueue intrptReqQueue_;
	array<SignalValue, static_cast<size_t> (Signal::Count)> controlSignals_;
	bool processorRunning_;
	u16	delay_;

	BusDevice *responsibleModule (u16 address);
	void pushInterruptRequest (InterruptRequest interruptReq);
};

#endif // !_QBUS_H_