#include "trace/trace.h"
#include "qbus.h"
#include "interruptrequest/interruptrequest.h"

#include <stdlib.h>
#include <string.h>

using std::shared_ptr;


#define	IRCJITTER()	(rand() % INTRPT_LATENCY_JITTER)

QBUS::QBUS ()
	:
	processorRunning_ {false}
{
	for (auto device : slots)
		device.reset ();
}

CondData<u16> QBUS::read (u16 address)
{
	u8 i;
	u16 addr = address;

	address &= 0xFFFE;

	for (i = 0; i < LSI11_SIZE; i++)
	{
		shared_ptr<BusDevice> module = slots[i];
		if (!module)
			continue;

		if (module->responsible (address))
		{
			u16 value;
			if (module->read (address, &value) == StatusCode::OK)
			{
				TRCBus (TRC_BUS_RD, addr, value);
				return value;
			}
			else
				return {};
		}
	}

	return {};
}


// Set an interrupt request. The only reason this could fail is when
// a device already has set an interrupt. That would be an error on the
// part of the device and wouldn't harm.
void QBUS::setInterrupt (TrapPriority priority, 
		unsigned char busOrder, unsigned char vector)
{
	InterruptRequest intrptReq {RequestType::IntrptReq, priority, busOrder, vector};
	pushInterruptRequest (intrptReq);
}

// Push the interrupt request created by setInterrupt or setTrap to the
// interupt queue.
void QBUS::pushInterruptRequest (InterruptRequest intrptReq)
{
	intrptReqQueue_.push (intrptReq);
	TRCIRQ (intrptReq.vector(), TRC_IRQ_OK);
	delay = IRCJITTER ();
}

// Clear the specified interrupt request. The InterruptRequQueue will delete
// the interrupt request equal to specified request. Equality is based on
// priority and busorder (see InterruptRequest::operator==).
void QBUS::clearInterrupt (TrapPriority priority, unsigned char busOrder)
{
	intrptReqQueue_.erase (InterruptRequest {RequestType::IntrptReq, 
		priority, busOrder, 0});
}

void QBUS::reset ()
{
	u8 i;

	TRCBus (TRC_BUS_RESET, 0, 0);

	/* Clear pending interrupts */
	intrptReqQueue_.clear();

	delay = 0;

	for (i = 0; i < LSI11_SIZE; i++)
	{
		shared_ptr<BusDevice> module = slots[i];
		if (!module)
			continue;

		module->reset ();
	}
}

// Wait a random number (max INTRPT_LATENCY) of steps till processing of an
// interrupt request. 
void QBUS::step ()
{
	++delay;
}

// Check if there is an interrupt request available that can be processed.
// Wait a random number (max INTRPT_LATENCY) of steps till processing of an
// interrupt request. The delay counter is incremented every QBUS step.
// Traps are handled internally in the CPU and are to be processed immediately.
bool QBUS::intrptReqAvailable() 
{
	return (!intrptReqQueue_.empty() && 
		(delay >= INTRPT_LATENCY || intrptReqQueue_.top().requestType() == RequestType::Trap));
}

// Return the priority of the interrupt request with the highest priority
u8 QBUS::intrptPriority()
{
	return static_cast<u8> (intrptReqQueue_.top().priority());
}

// Get the interrupt request with the highest priority if one is available
bool QBUS::getIntrptReq(InterruptRequest &intrptReq)
{
	if (intrptReqAvailable())
	{
		bool result = intrptReqQueue_.fetchTop (intrptReq);
		TRCIRQ (intrptReq.vector(), TRC_IRQ_SIG);
		delay = 0;
		return result;
	}
	else
		return false;
}

void QBUS::installModule (int slot, shared_ptr<BusDevice> module)
{
	slots[slot] = module;
	module->bus = this;
}

// The functions setProcessorRunning and processorIsRunning abstract the
// SRUN L signal. SRUN L, a non-bused, backplane signal, is a series of
// pulses which occur at 3-5",s intervals whenever the processor is in
// the Run mode.
void QBUS::setProcessorRunning (bool running)
{
	processorRunning_ = running;
}


bool QBUS::processorIsRunning ()
{
	return processorRunning_;
}
