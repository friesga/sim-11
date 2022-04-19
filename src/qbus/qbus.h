#ifndef _QBUS_H_
#define _QBUS_H_

#include "types.h"
#include "../conddata/conddata.h"
#include "../interruptrequest/interruptrequest.h"

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
	void	reset ();
	void	step ();
	CondData<u16> read (u16 addr);
	void	write (u16 addr, u16 value);
	void	installModule (int slot, QBUSModule* module);

	QBUSModule*	slots[LSI11_SIZE];
	u16	delay;
	InterruptRequest trap;
	InterruptRequest irq;

private:
	InterruptRequest const busError{TrapPriority::BusError, 0, 004};
	InterruptRequest const emptyIntrptReq{TrapPriority::None, 0, 0};
};

// Define the functions every QBUS module should provide
class QBUSModule
{
public:
	QBUS*	bus;
	u16		virtual read (u16 addr) = 0;
	void	virtual write (u16 addr, u16 value) = 0;
	u8		virtual responsible (u16 addr) = 0;
	void	virtual reset () = 0;

	InterruptRequest	irq;
};

#endif // !_QBUS_H_