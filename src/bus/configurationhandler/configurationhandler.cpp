#include "configurationhandler.h"

#include <algorithm>

using std::copy_backward;

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
bool ConfigurationHandler::installModuleAtPosition (BusDevice* device, size_t position)
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

bool ConfigurationHandler::installModule (BusDevice* device)
{
	if (device == nullptr || numDevices_ >= numberOfSlots)
		return false;

	slots_[numDevices_++] = device;
	return true;
}

void ConfigurationHandler::installUnibusMap (UnibusMap* device)
{
	unibusMap_ = device;
}

Iterator ConfigurationHandler::begin ()
{
    return Iterator (&slots_[0]);
}
    
Iterator ConfigurationHandler::end ()
{
    return Iterator (&slots_[numDevices_ - 1]);
}

Iterator ConfigurationHandler::operator[] (int index)
{ 
    return Iterator (&slots_[index]);
}

void ConfigurationHandler::reset ()
{
	// Reset all devices on the bus
	for (BusDevice* module : slots_)
	{
		if (module != nullptr)
			module->reset ();
	}
}