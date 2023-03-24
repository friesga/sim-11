#include "lsi11.h"

void LSI11::installModules ()
{
    bus_.installModule (1, kd11_);
    bus_.installModule (2, msv11_);
    bus_.installModule (3, rlv12_);
    bus_.installModule (4, rxv21_);
    bus_.installModule (5, dlv11_);
    bus_.installModule (6, bdv11_);
}