#include "pdp11.h"

// Install the devices on the bus from the configured processor, memory
// devices, m9312 and other bus devices. The installation is kept in one
// function so the address resolution priority of the devices is determined
// in one place.
// 
// A BA11-N backplane has nine slots, named ROW 1 to ROW 9. ROW 1 corresponds
// with slot[0], ROW 9 with slot [8].
void PDP_11::installModules ()
{
    // Install a possibly present M9312 or KT24 as the first device on the bus
    // so it has the highest priority for address resolution on the bus.
    // A Unibus configuration will contain either a M9312 or a KT24 but not
    // both
    if (m9312_ != nullptr)
        bus_.installModule (m9312_);

    if (kt24_ != nullptr)
        bus_.installModule (kt24_);
    
    for (auto device : memoryDevices_)
        bus_.installModule (device);

    for (auto device : busDevices_)
        bus_.installModule (device);

    bus_.installModule (processor_);
}
