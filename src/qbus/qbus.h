#ifndef _QBUS_H_
#define _QBUS_H_

#include "types.h"
#include "statuscodes.h"
#include "conddata/conddata.h"
#include "interruptrequest/interruptrequest.h"
#include "threadsafeprioqueue/threadsafeprioqueue.h"

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
	bool	write8 (u16 addr, u8 val);
	void	installModule (int slot, QBUSModule* module);

	QBUSModule*	slots[LSI11_SIZE];
	u16	delay;

private:
	// This queue keeps all interrupt requests, ordered in interrupt priority
	using IntrptReqQueue = ThreadSafePrioQueue<InterruptRequest>;
	IntrptReqQueue intrptReqQueue_;

	InterruptRequest const busError{RequestType::Trap, TrapPriority::BusError, 0, 004};
};

// Define the functions every QBUS module should provide
class QBUSModule
{
public:
	QBUS*	bus;
	virtual StatusCode read (u16 addr, u16 *destination) = 0;
	void	virtual write (u16 addr, u16 value) = 0;
	u8		virtual responsible (u16 addr) = 0;
	void	virtual reset () = 0;
};

#endif // !_QBUS_H_