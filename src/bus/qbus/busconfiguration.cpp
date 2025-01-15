#include "qbus.h"

bool Qbus::installModuleAtPosition (BusDevice* module, size_t position)
{
    return configurationHandler_.installModuleAtPosition (module, position);
}

bool Qbus::installModule (BusDevice* module)
{
    return configurationHandler_.installModule (module);
}

void Qbus::installUnibusMap (UnibusMap* device)
{
    return configurationHandler_.installUnibusMap (device);
}

BusDevice* Qbus::responsibleModule (BusAddress address)
{
    return configurationHandler_.responsibleModule (address);
}

Iterator Qbus::begin ()
{
    return configurationHandler_.begin ();
}

Iterator Qbus::end ()
{
    return configurationHandler_.end ();
}

Iterator Qbus::operator[] (int index)
{
    return configurationHandler_.operator[] (index);
}
