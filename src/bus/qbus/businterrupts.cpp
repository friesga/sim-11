#include "qbus.h"

using std::function;

void Qbus::busRequest (TrapPriority priority, unsigned char busOrder,
	u8 functionOrder, function<void ()> busGrant)
{


}

void Qbus::setInterrupt (TrapPriority priority, unsigned char busOrder,
	u8 functionOrder, unsigned char vector, function<void ()> ack)
{
	return interruptHandler_.setInterrupt (priority, busOrder, functionOrder,
		vector, ack);
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
