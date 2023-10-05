#include "lsi11.h"

// Install the devices on the bus. The installation is kept in one function
// to easily determine the order of devices on the bus.
void LSI11::installModules ()
{
    // Install either the KD11-NA or the KDF11-A processor.
    bus_.installModule (1, processor_);
    bus_.installModule (2, msv11_);
    bus_.installModule (3, rlv12_);
    bus_.installModule (4, rxv21_);
    bus_.installModule (5, dlv11_);
    bus_.installModule (6, bdv11_);
}