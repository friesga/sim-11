#include "unibus.h"
#include "trace/trace.h"

bool Unibus::installModuleAtPosition (BusDevice* module, size_t position)
{
    return configurationHandler_.installModuleAtPosition (module, position);
}

// If the given module is a Unibus Map option save the pointer to this
// specific module. The dynamic_cast operator will retun a pointer to the
// module if it implements the UnibusMap or null if that interface is not
// supported.
bool Unibus::installModule (BusDevice* module)
{
    if (dynamic_cast<UnibusMap*> (module) != nullptr)
        unibusMap_ = static_cast<UnibusMap*> (module);

    return configurationHandler_.installModule (module);
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
