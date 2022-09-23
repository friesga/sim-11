#ifndef _QBUS_H_
#define _QBUS_H_

#include "types.h"
#include "statuscodes.h"
#include "conddata/conddata.h"
#include "interruptrequest/interruptrequest.h"
#include "threadsafecontainers/threadsafeprioqueue.h"
#include "busdevice/busdevice.h"

/* Backplane size */
#define	LSI11_SIZE		8

// QBUS interrupt latency, defined as the maximum number of instructions
// after which the interrupt will be processed. The INTRPT_LATENCY_JITTER
// is a random number of instructions. The minimum latency will be
// INTRPT_LATENCY - INTRPT_LATENCY_JITTER and the maximum latency is the
// value of INTRPT_LATENCY.
// 
// According to the Microcomputer Processor Handbook, the interrupt
// latency lies between 35.05 and 44.1 +/- 20%, so roughly between 30 and
// 50 microseconds. The LSI-11 instruction time varies between 3.5
// and 15 microseconds. If we presume an averige execution time of 5 micro-
// seconds, an interrupt latency between 6 and 10 instructions would be 
// appropriate. An interrupt latency of 20 instructions will result in
// the VRLBC0 diagnostic reporting "NO INTERRUPT ON FUNCTION COMPLETE" errors.
//  
#define	INTRPT_LATENCY			11
#define	INTRPT_LATENCY_JITTER	5

class BusDevice;

class QBUS
{
public:
	QBUS ();
	void	setInterrupt (TrapPriority priority, 
				unsigned char busOrder, unsigned char vector);
	void	clearInterrupt (TrapPriority priority, 	unsigned char busOrder);
	bool	intrptReqAvailable();
	u8		intrptPriority();
	bool	getIntrptReq(InterruptRequest &ir);
	void	reset ();
	void	step ();
	CondData<u16> read (u16 addr);
	bool	writeWord (u16 addr, u16 value);
	bool	writeByte (u16 addr, u8 val);
	void	installModule (int slot, BusDevice* module);

	BusDevice*	slots[LSI11_SIZE];
	u16	delay;

private:
	// This queue keeps all interrupt requests, ordered in interrupt priority
	using IntrptReqQueue = ThreadSafePrioQueue<InterruptRequest>;
	IntrptReqQueue intrptReqQueue_;

	BusDevice *responsibleModule (u16 address);
	void pushInterruptRequest (InterruptRequest interruptReq);
};

#endif // !_QBUS_H_