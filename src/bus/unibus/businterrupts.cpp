#include "unibus.h"

using std::function;

void Unibus::requestInterrupt (TrapPriority priority, unsigned char busOrder,
	u8 functionOrder, function<u16 ()> requestGrant)
{
	return interruptHandler_.setInterrupt (priority, busOrder, functionOrder,
		requestGrant ());
}

bool Unibus::containsInterrupt (TrapPriority priority, unsigned char busOrder,
	u8 functionOrder)
{
	return interruptHandler_.containsInterrupt (priority, busOrder,
		functionOrder);
}

void Unibus::clearInterrupt (TrapPriority priority, unsigned char busOrder,
	u8 functionOrder)
{
	return interruptHandler_.clearInterrupt (priority, busOrder, functionOrder);
}

void Unibus::clearInterrupts ()
{
	return interruptHandler_.clearInterrupts ();
}

bool Unibus::intrptReqAvailable ()
{
	return interruptHandler_.intrptReqAvailable ();
}

u8 Unibus::intrptPriority ()
{
	return interruptHandler_.intrptPriority ();
}

bool Unibus::getIntrptReq (InterruptRequest& intrptReq)
{
	return interruptHandler_.getIntrptReq (intrptReq);
}
