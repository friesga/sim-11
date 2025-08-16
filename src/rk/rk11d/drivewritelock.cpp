#include "rk11d.h"
#include "rk/include/rktypes.h"

void RK11D::driveWriteLock (RKTypes::Function function)
{
    rk05Drives_[function.diskAddress.driveSelect]->writeLock ();
}