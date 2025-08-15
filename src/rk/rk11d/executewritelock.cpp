#include "rk11d.h"
#include "rk/include/rktypes.h"

#include <algorithm>

using std::ranges::all_of;

// The Write Lock function write-protects a selected disk drive until the
// condition is overridden by operation of the corresponding WT PROT
// (Write Protect) switch on the disk drive (refer to RK05 Disk Drive
// Maintenance Manual, DEC-00-HRKO05-C-D). The disk drive is automatically
// write-enabled when power is first applied, or when the disk drive RUN/LOAD
// switch is set to RUN.
// (EK-RK11D-MM-002, par. 1.3.2.4)
//
void RK11D::executeWriteLock (RKTypes::Function function)
{
    RKTypes::CommandCompletion commandCompletion {};

    all_of (writeLockFunction_, [&] (auto& f)
        { return f (function, commandCompletion); });
}

void RK11D::driveWriteLock (RKTypes::Function function)
{
    rk05Drives_[function.diskAddress.driveSelect]->writeLock ();
}