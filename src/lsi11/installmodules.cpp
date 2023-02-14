#include "lsi11.h"

void LSI11::installModules ()
{
    // The Console class reads characters and sends them to the dlv11
    console_ = Console::create (dlv11_);

    bus_.installModule (1, kd11_);
    bus_.installModule (2, msv11_);
    bus_.installModule (3, rlv12_);
    bus_.installModule (4, rxv21_);
    bus_.installModule (5, dlv11_);
    bus_.installModule (6, bdv11_);
}