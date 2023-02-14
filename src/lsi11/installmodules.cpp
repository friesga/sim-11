#include "lsi11.h"

void LSI11::installModules ()
{
    // The Console class reads characters and sends them to the dlv11
    console_ = Console::create (dlv11_);

    bus.installModule (1, kdv11_);
    bus.installModule (2, msv11_);
    bus.installModule (3, rlv12_);
    bus.installModule (4, rxv21_);
    bus.installModule (5, dlv11_);
    bus.installModule (6, bdv11_);
}