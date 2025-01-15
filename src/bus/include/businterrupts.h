#ifndef _BUSINTERRUPTS_H_
#define _BUSINTERRUPTS_H_

#include "interruptrequest/interruptrequest.h"

// This interfaces declares the bus functions for interrupt handling
class BusInterrupts
{
public:
	virtual void setInterrupt (TrapPriority priority, unsigned char busOrder,
		u8 functionOrder, unsigned char vector) = 0;
	virtual bool containsInterrupt (TrapPriority priority, unsigned char busOrder,
		u8 functionOrder) = 0;
	virtual void clearInterrupt (TrapPriority priority, unsigned char busOrder,
		u8 functionOrder) = 0;
	virtual void clearInterrupts () = 0;
	virtual bool intrptReqAvailable () = 0;
	virtual u8 intrptPriority () = 0;
	virtual bool getIntrptReq (InterruptRequest& ir) = 0;
};

#endif // _BUSINTERRUPTS_H_