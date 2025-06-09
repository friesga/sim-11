#ifndef _BUSINTERRUPTS_H_
#define _BUSINTERRUPTS_H_

#include "interruptrequest/interruptrequest.h"

#include <functional>

using std::function;

// This interfaces declares the bus functions for interrupt handling.
// 
// To set an interrupt request a device calls setInterrupt() and these
// interrupts will then be handled in order of their priority. Bus arbitration
// is thus performed implicitcly.
// 
// Bus arbitration can also be performed explicitly by calling busRequest().
// This function includes a busGrant callback function wich will be called
// when an interrupt request with the given bus request priority will be
// honoured.
//
class BusInterrupts
{
public:
	virtual void busRequest (TrapPriority priority, unsigned char busOrder,
		u8 functionOrder, function<void ()> busGrant = 0) = 0;
	virtual void setInterrupt (TrapPriority priority, unsigned char busOrder,
		u8 functionOrder, unsigned char vector, function<void()> ack = 0) = 0;
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