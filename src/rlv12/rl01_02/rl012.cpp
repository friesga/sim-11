#include "rl012.h"
#include "../rlv12.h"

// Constructor
// By default the unit is off-line. It is set on-line when a file is
// attached.
RL01_2::RL01_2 (BusDevice *owningDevice)
    :
    Unit (owningDevice),
    currentDiskAddress_ {0},
    rlStatus_ {RlStatus::UNIT_OFFL},
    driveStatus_ {0},
    function_ {0}
{}

