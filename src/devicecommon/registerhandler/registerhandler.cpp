#include "registerhandler.h"

RegisterHandler::RegisterHandler (vector<BusDevice*> cpuModules)
    :
    cpuModules_ {cpuModules}
{}

CondData<u16> RegisterHandler::read (BusAddress address)
{
    for (BusDevice* module : cpuModules_)
        if (module->responsible (address))
            return module->read (address);

    return {StatusCode::NonExistingMemory};
}

StatusCode RegisterHandler::writeWord (BusAddress address, u16 value)
{
    for (BusDevice* module : cpuModules_)
        if (module->responsible (address))
            return module->writeWord (address, value);

    return StatusCode::NonExistingMemory;

}

bool RegisterHandler::responsible (BusAddress busAddress)
{
    if (!busAddress.isInIOpage ())
        return false;

    for (BusDevice* module : cpuModules_)
        if (module->responsible (busAddress))
            return true;

    return false;
}

void RegisterHandler::reset ()
{
    for (BusDevice* module : cpuModules_)
        module->reset ();
}

