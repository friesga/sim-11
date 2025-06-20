#ifndef _BUSINTERRUPTS_H_
#define _BUSINTERRUPTS_H_

#include "bus/interruptrequest/interruptrequest.h"

#include <functional>

using std::function;

// This interfaces declares the bus functions for interrupt handling.
//
// The requestInterrupt() function calls the requestGrant() callback when the
// requested interrupt is to be processed. This vector should then retun the
// interrupt vector. This setup allows devices to set e.g. the device
// registers to the correct state.
//
class BusInterrupts
{
public:
	virtual void requestInterrupt (TrapPriority priority, unsigned char busOrder,
		u8 functionOrder, u16 vector, function<void ()> requestGrant = 0) = 0;
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