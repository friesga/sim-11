#include "interrupthandler.h"
#include "trace/trace.h"

// Set an interrupt request. To allow synchronization between multiple
// interrupt request from the same device an interrupt request can be
// acknowledged, indicating to the device the interrupt
void InterruptHandler::setInterrupt (TrapPriority priority,
	unsigned char busOrder, u8 functionOrder, u16 vector, 
	function<void ()> requestGrant)
{
	InterruptRequest intrptReq {priority, busOrder, functionOrder,
		vector, requestGrant};
	pushInterruptRequest (intrptReq);
}

// Push the interrupt request created by setInterrupt or setTrap to the
// interupt queue.
void InterruptHandler::pushInterruptRequest (InterruptRequest intrptReq)
{
	intrptReqQueue_.push (intrptReq);
	trace.irq (IrqRecordType::IRQ_REQUEST, intrptReq);
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
	IntrptReqQueue::ConstIterator it = intrptReqQueue_.find (
        InterruptRequest {priority, busOrder, functionOrder, 0});

	if (it != intrptReqQueue_.cend ())
	{
		trace.irq (IrqRecordType::IRQ_CLEAR, *it);
		intrptReqQueue_.erase (it);
	}
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
// and grant the request.
bool InterruptHandler::getIntrptReq (InterruptRequest& intrptReq)
{
	if (intrptReqAvailable ())
	{
		bool result = intrptReqQueue_.fetchTop (intrptReq);
		intrptReq.requestGrant ();
		trace.irq (IrqRecordType::IRQ_GRANT, intrptReq);
		return result;
	}
	else
		return false;
}

