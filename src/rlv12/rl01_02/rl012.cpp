#include "rl012.h"
#include "../rlv12.h"

// Constructor
RL01_2::RL01_2 (BusDevice *owningDevice)
    :
    Unit (owningDevice),
    currentDiskAddress_ {0},
    rlStatus_ {RlStatus::UNIT_AUTO},
    driveStatus_ {0}
{}

