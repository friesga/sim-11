#ifndef _QBUS_H_
#define _QBUS_H_

#include "types.h"
#include "statuscodes.h"
#include "conddata/conddata.h"
#include "interruptrequest/interruptrequest.h"
#include "threadsafeprioqueue/threadsafeprioqueue.h"
#include "busdevice/busdevice.h"

/* Backplane size */
#define	LSI11_SIZE		8

// QBUS interrupt latency, defined as the maximum number of instructions
// after which the interrupt will be processed.
// 
// According to the Microcomputer Processor Handbook, the maximum interrupt
// latency (with KEV11 option present) is 44.1 microseconds +/- 20%, so 
// roughly 50 micoseconds. The LSI-11 instruction time varies between 3.5
// and 15 microseconds. If we presume an averige execution time of 5 micro-
// seconds, a maximum interrupt latency of 10 instructions would be 
// appropriate. An interrupt latency of 20 instructions will result in
// the VRLBC0 diagnostic reporting "NO INTERRUPT ON FUNCTION COMPLETE" errors.
//  
#define	INTRPT_LATENCY		10

/* QBUS interrupt request delay jitter */
#define	QBUS_DELAY_JITTER	10

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