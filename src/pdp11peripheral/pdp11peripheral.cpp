#include "pdp11peripheral.h"

PDP11Peripheral::PDP11Peripheral (Qbus *bus)
    :
    bus_ {bus},
    baseAddress_ {0},
    vector_ {0}
{}
