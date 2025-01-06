#include "qbus.h"

CondData<u16> Qbus::dmaRead (BusAddress address)
{
    return read (address);
}

bool Qbus::dmaWrite (BusAddress address, u16 value)
{
    return writeWord (address, value);
}