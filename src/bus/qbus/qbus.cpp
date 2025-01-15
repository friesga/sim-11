#include "trace/trace.h"
#include "qbus.h"
#include "interruptrequest/interruptrequest.h"

#include <stdlib.h>
#include <string.h>
#include <algorithm>

using std::bind;
using std::placeholders::_1;
using std::copy_backward;

// On the Unibus and QBus reads are always word-sized (and from an even
// address) and writes may be either word or byte sized (with byte-sized
// writes using the upper or lower byte depending on whether the address
// is odd or even). When the CPU needs a byte it reads a word and ignores
// the portion of the word it isn't interested in.
// 
// Source: https://retrocomputing.stackexchange.com/questions/13262/how-movb-tstb-and-all-byte-instructions-works-in-odd-address-read-in-pdp11
//
Qbus::Qbus ()
	:
	processorRunning_ {false}
{
	ourKey_ = BINIT().subscribe (bind (&Qbus::BINITReceiver, this, _1));
}

CondData<u16> Qbus::read (BusAddress address)
{
	BusDevice *module;

	// Prevent read's from odd addresses
	address &= 0xFFFFFFFE;

	if ((module = responsibleModule (address)) != nullptr)
	{
		u16 value;
		if (module->read (address, &value) == StatusCode::OK)
		{
			trace.bus (BusRecordType::Read, address, value);
			return value;
		}
		else
			return {};
	}

	return {};
}


// The bus itself is defined as a BINIT signal receiver too as it not
// only has to pass on the signal to all subscribed devices, but has to
// perform actions itself too.
void Qbus::BINITReceiver (bool signalValue)
{
	if (signalValue)
		reset ();
}

void Qbus::reset ()
{
	trace.bus (BusRecordType::BusReset, 0, 0);

	// Clear pending interrupts
	clearInterrupts ();

	// Reset all devices on the bus
	for (BusDevice* module : slots_)
	{
		if (module != nullptr)
			module->reset ();
	}
}



// Install the given device at the specified position.  This implies
// that first the already installed devices have to be shifted one position.
// 
// The first device on the bus has the highest priority for determining the
// device responsible for a given bus address. The last device installed at
// the front of the bus thus has the highest priority. This functionality
// is used to give the M9312 priority for access to the powerfail vector.
//
// Devices must be installed consecutively in the slots for the iterator
// to work correctly.
//
bool Qbus::installModuleAtPosition (BusDevice* device, size_t position)
{
	if (device == nullptr || 
		numDevices_ >= numberOfSlots ||
		position > numDevices_)
			return false;

	// Copy the elements from the given position till numDevices_ to the given
	// positon + 1 till the range ending at numDevices_ + 1 whereby the last
	// element is copied first.
	copy_backward (slots_ + position, slots_ + numDevices_,
		slots_ + numDevices_ + 1);
	slots_[position] = device;
	++numDevices_;
	return true;
}

// Install the given bus device at the first free position
bool Qbus::installModule (BusDevice* device)
{
	if (device == nullptr || numDevices_ >= numberOfSlots)
        return false;

	slots_[numDevices_++] = device;
	return true;
}

void Qbus::installUnibusMap (UnibusMap* device)
{
	unibusMap_ = device;
}