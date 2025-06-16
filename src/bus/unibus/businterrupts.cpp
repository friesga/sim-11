#include "unibus.h"

using std::function;

void Unibus::setInterrupt (TrapPriority priority, unsigned char busOrder,
	u8 functionOrder, unsigned char vector, function<void ()> ack)
{
	return interruptHandler_.setInterrupt (priority, busOrder, functionOrder,
		vector, ack);
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
