#include "registeraccess.h"

RegisterAccess::RegisterAccess (vector<BusDevice*> cpuModules)
    :
    cpuModules_ {cpuModules}
{}

StatusCode RegisterAccess::read (BusAddress address, u16* destination)
{
    for (BusDevice* module : cpuModules_)
        if (module->responsible (address))
            return module->read (address, destination);

    return StatusCode::NonExistingMemory;
}

StatusCode RegisterAccess::writeWord (BusAddress address, u16 value)
{
    for (BusDevice* module : cpuModules_)
        if (module->responsible (address))
            return module->writeWord (address, value);

    return StatusCode::NonExistingMemory;

}

bool RegisterAccess::responsible (BusAddress busAddress)
{
    if (!busAddress.isInIOpage ())
        return false;

    for (BusDevice* module : cpuModules_)
        if (module->responsible (busAddress))
            return true;

    return false;
}

void RegisterAccess::reset ()
{
    for (BusDevice* module : cpuModules_)
        module->reset ();
}

