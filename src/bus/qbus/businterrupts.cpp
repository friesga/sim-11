#include "qbus.h"

using std::function;

void Qbus::requestInterrupt (TrapPriority priority, unsigned char busOrder,
	u8 functionOrder, u16 vector, function<void ()> requestGrant)
{
	return interruptHandler_.setInterrupt (priority, busOrder, functionOrder,
		vector, requestGrant);
}

bool Qbus::containsInterrupt (TrapPriority priority, unsigned char busOrder,
	u8 functionOrder)
{
	return interruptHandler_.containsInterrupt (priority, busOrder,
		functionOrder);
}

void Qbus::clearInterrupt (TrapPriority priority, unsigned char busOrder,
	u8 functionOrder)
{
	return interruptHandler_.clearInterrupt (priority, busOrder, functionOrder);
}

void Qbus::clearInterrupts ()
{
	return interruptHandler_.clearInterrupts ();
}

bool Qbus::intrptReqAvailable ()
{
	return interruptHandler_.intrptReqAvailable ();
}

u8 Qbus::intrptPriority ()
{
	return interruptHandler_.intrptPriority ();
}

bool Qbus::getIntrptReq (InterruptRequest& intrptReq)
{
	return interruptHandler_.getIntrptReq (intrptReq);
}
