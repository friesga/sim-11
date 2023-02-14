#include "busdevice.h"

BusDevice::BusDevice (QBUS *bus)
    :
    bus_ {bus},
    baseAddress_ {0},
    vector_ {0}
{}
