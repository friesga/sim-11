#include "trace/trace.h"
#include "qbus.h"
#include "interruptrequest/interruptrequest.h"

#include <stdlib.h>
#include <string.h>

#define	IRCJITTER()	(rand() % INTRPT_LATENCY_JITTER)

Qbus::Qbus ()
	:
	controlSignals_ {false},
	processorRunning_ {false},
	delay_ {0}
{
	for (auto device : slots)
	{
		if (device != nullptr)
			device->reset ();
	}
}

CondData<u16> Qbus::read (u16 address)
{
	BusDevice *module;
	u16 addr = address;

	address &= 0xFFFE;

	if ((module = responsibleModule (address)) != nullptr)
	{
		u16 value;
		if (module->read (address, &value) == StatusCode::OK)
		{
			trace.bus (BusRecordType::Read, addr, value);
			return value;
		}
		else
			return {};
	}

	return {};
}

// Set an interrupt request. The only reason this could fail is when
// a device already has set an interrupt. That would be an error on the
// part of the device and wouldn't harm.
void Qbus::setInterrupt (TrapPriority priority, 
		unsigned char busOrder, unsigned char vector)
{
	InterruptRequest intrptReq {RequestType::IntrptReq, priority, busOrder, vector};
	pushInterruptRequest (intrptReq);
}

// Push the interrupt request created by setInterrupt or setTrap to the
// interupt queue.
void Qbus::pushInterruptRequest (InterruptRequest intrptReq)
{
	intrptReqQueue_.push (intrptReq);
	trace.irq (IrqRecordType::IRQ_OK, intrptReq.vector());
	delay_ = IRCJITTER ();
}

// Clear the specified interrupt request. The InterruptRequQueue will delete
// the interrupt request equal to specified request. Equality is based on
// priority and busorder (see InterruptRequest::operator==).
void Qbus::clearInterrupt (TrapPriority priority, unsigned char busOrder)
{
	intrptReqQueue_.erase (InterruptRequest {RequestType::IntrptReq, 
		priority, busOrder, 0});
}

void Qbus::reset ()
{
	u8 i;

	trace.bus (BusRecordType::BusReset, 0, 0);

	/* Clear pending interrupts */
	intrptReqQueue_.clear();

	delay_ = 0;

	for (i = 0; i < LSI11_SIZE; i++)
	{
		BusDevice *module = slots[i];
		if (!module)
			continue;

		module->reset ();
	}
}

// Wait a random number (max INTRPT_LATENCY) of steps till processing of an
// interrupt request. 
void Qbus::step ()
{
	++delay_;
}

// Check if there is an interrupt request available that can be processed.
// Wait a random number (max INTRPT_LATENCY) of steps till processing of an
// interrupt request. The delay counter is incremented every Qbus step.
// Traps are handled internally in the CPU and are to be processed immediately.
bool Qbus::intrptReqAvailable() 
{
	return (!intrptReqQueue_.empty() && 
		(delay_ >= INTRPT_LATENCY || intrptReqQueue_.top().requestType() == RequestType::Trap));
}

// Return the priority of the interrupt request with the highest priority
u8 Qbus::intrptPriority()
{
	return static_cast<u8> (intrptReqQueue_.top().priority());
}

// Get the interrupt request with the highest priority if one is available
bool Qbus::getIntrptReq(InterruptRequest &intrptReq)
{
	if (intrptReqAvailable())
	{
		bool result = intrptReqQueue_.fetchTop (intrptReq);
		trace.irq (IrqRecordType::IRQ_SIG, intrptReq.vector());
		delay_ = 0;
		return result;
	}
	else
		return false;
}

// Install the given device in the given slot. Every BusDevice contains a
// pointer to the bus it is installed on. This pointer has to be specified
// in the device's constructor. There is something to be said for setting
// the pointer in the installModule function but in e.g. the KD11 device
// the pointer is already used in the constructor.
void Qbus::installModule (int slot, BusDevice *module)
{
	if (slot < LSI11_SIZE && module != nullptr)
		slots[slot] = module;
}

BusDevice *Qbus::findModuleByName (std::string moduleName)
{
	for (auto module : slots)
		if (module != nullptr && module->name() == moduleName)
			return module;
	return nullptr;
}