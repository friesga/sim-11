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

/* QBUS interrupt request delay */
#define	QBUS_DELAY		20

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
	void	setTrap (TrapPriority priority, unsigned char vector);
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