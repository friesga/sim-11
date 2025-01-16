#include "unibus.h"
#include "trace/trace.h"

bool Unibus::installModuleAtPosition (BusDevice* module, size_t position)
{
    return configurationHandler_.installModuleAtPosition (module, position);
}

bool Unibus::installModule (BusDevice* module)
{
    return configurationHandler_.installModule (module);
}

void Unibus::installUnibusMap (UnibusMap* device)
{
    return configurationHandler_.installUnibusMap (device);
}

BusDevice* Unibus::responsibleModule (BusAddress address)
{
    return configurationHandler_.responsibleModule (address);
}

Iterator Unibus::begin ()
{
    return configurationHandler_.begin ();
}

Iterator Unibus::end ()
{
    return configurationHandler_.end ();
}

size_t Unibus::capacity ()
{
    return configurationHandler_.capacity ();
}

Iterator Unibus::operator[] (int index)
{
    return configurationHandler_.operator[] (index);
}

void Unibus::reset ()
{
    trace.bus (BusRecordType::BusReset, 0, 0);

    // Clear pending interrupts
    clearInterrupts ();

    // Reset all devices on the bus
    configurationHandler_.reset ();
}
