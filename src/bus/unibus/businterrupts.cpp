#include "unibus.h"

using std::function;

void Unibus::requestInterrupt (InterruptPriority priority, unsigned char busOrder,
	u8 functionOrder, u16 vector, function<void ()> requestGrant)
{
	return interruptHandler_.setInterrupt (priority, busOrder, functionOrder,
		vector, requestGrant);
}

bool Unibus::containsInterrupt (InterruptPriority priority, unsigned char busOrder,
	u8 functionOrder)
{
	return interruptHandler_.containsInterrupt (priority, busOrder,
		functionOrder);
}

void Unibus::clearInterrupt (InterruptPriority priority, unsigned char busOrder,
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
