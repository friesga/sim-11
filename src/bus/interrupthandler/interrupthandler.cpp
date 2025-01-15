#include "interrupthandler.h"
#include "trace/trace.h"

// Set an interrupt request. The only reason this could fail is when
// a device already has set an interrupt. That would be an error on the
// part of the device and wouldn't harm.
void InterruptHandler::setInterrupt (TrapPriority priority,
	unsigned char busOrder, u8 functionOrder, unsigned char vector)
{
	InterruptRequest intrptReq {priority, busOrder, functionOrder, vector};
	pushInterruptRequest (intrptReq);
}

// Push the interrupt request created by setInterrupt or setTrap to the
// interupt queue.
void InterruptHandler::pushInterruptRequest (InterruptRequest intrptReq)
{
	intrptReqQueue_.push (intrptReq);
	trace.irq (IrqRecordType::IRQ_OK, intrptReq.vector ());
}

bool InterruptHandler::containsInterrupt (TrapPriority priority, unsigned char busOrder,
	u8 functionOrder)
{
	return intrptReqQueue_.contains (InterruptRequest {priority,
		busOrder, functionOrder, 0});
}

// Clear the specified interrupt request. The InterruptRequQueue will delete
// the interrupt request equal to specified request. Equality is based on
// priority and busorder (see InterruptRequest::operator==).
void InterruptHandler::clearInterrupt (TrapPriority priority, unsigned char busOrder,
	u8 functionOrder)
{
	intrptReqQueue_.erase (InterruptRequest {priority, busOrder, functionOrder, 0});
}

// Clear all pending interrupts
void InterruptHandler::clearInterrupts ()
{
	intrptReqQueue_.clear ();
}

bool InterruptHandler::intrptReqAvailable ()
{
	return (!intrptReqQueue_.empty ());
}

// Return the priority of the interrupt request with the highest priority
u8 InterruptHandler::intrptPriority ()
{
	return static_cast<u8> (intrptReqQueue_.top ().priority ());
}

// Get the interrupt request with the highest priority if one is available
bool InterruptHandler::getIntrptReq (InterruptRequest& intrptReq)
{
	if (intrptReqAvailable ())
	{
		bool result = intrptReqQueue_.fetchTop (intrptReq);
		trace.irq (IrqRecordType::IRQ_SIG, intrptReq.vector ());
		return result;
	}
	else
		return false;
}
