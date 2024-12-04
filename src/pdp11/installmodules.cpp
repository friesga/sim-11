#include "pdp11.h"

// Install the devices on the bus. The installation is kept in one function
// to easily determine the order of devices on the bus.
// A BA11-N backplane has nine slots, named ROW 1 to ROW 9. ROW 1 corresponds
// with slot[0], ROW 9 with slot [8].
void PDP_11::installModules ()
{
    size_t slotNr {0};

    bus_.installModule (slotNr++, processor_);

    for (auto device : busDevices_)
        bus_.installModule (slotNr++, device);

    for (auto device : memoryDevices_)
        bus_.installModule (slotNr++, device);
    
}