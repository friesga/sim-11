#include "qbus.h"

CondData<u16> Qbus::dmaRead (BusAddress address)
{
    /*
    return (unibusMap_ != nullptr) ? 
        unibusMap_->dmaRead (address) : read (address);
    */

    return read (address);
}

bool Qbus::dmaWrite (BusAddress address, u16 value)
{
    /*
    return (unibusMap_ != nullptr) ?
        unibusMap_->dmaWrite (address, value) : writeWord (address, value);
    */

    return writeWord (address, value);
}