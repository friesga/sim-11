#include "rl012.h"
#include "../rlv12.h"

// Constructor
RL01_2::RL01_2 (BusDevice *owningDevice)
    :
    Unit (owningDevice),
    currentTrackHeadSector_ {0}
{}

