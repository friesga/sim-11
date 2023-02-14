#include "busdevice.h"

BusDevice::BusDevice (Qbus *bus)
    :
    bus_ {bus},
    baseAddress_ {0},
    vector_ {0}
{}
