#ifndef _QBUS_H_
#define _QBUS_H_

#include "types.h"
#include "statuscodes.h"
#include "conddata/conddata.h"
#include "interruptrequest/interruptrequest.h"
#include "threadsafeprioqueue/threadsafeprioqueue.h"
#include "qbusmodule/qbusmodule.h"

/* Backplane size */
#define	LSI11_SIZE		8

/* QBUS interrupt request delay */
#define	QBUS_DELAY		20

/* QBUS interrupt request delay jitter */
#define	QBUS_DELAY_JITTER	10

class QBUSModule;

class QBUS
{
public:
	QBUS ();
	int		interrupt (InterruptRequest intrptReq);
	bool	intrptReqAvailable();
	u8		intrptPriority();
	bool	getIntrptReq(InterruptRequest &ir);
	void	reset ();
	void	step ();
	CondData<u16> read (u16 addr);
	bool	writeWord (u16 addr, u16 value);
	bool	writeByte (u16 addr, u8 val);
	void	installModule (int slot, QBUSModule* module);

	QBUSModule*	slots[LSI11_SIZE];
	u16	delay;

private:
	// This queue keeps all interrupt requests, ordered in interrupt priority
	using IntrptReqQueue = ThreadSafePrioQueue<InterruptRequest>;
	IntrptReqQueue intrptReqQueue_;

	InterruptRequest const busError{RequestType::Trap, TrapPriority::BusError, 0, 004};

	QBUSModule *responsibleModule (u16 address);
};

#endif // !_QBUS_H_