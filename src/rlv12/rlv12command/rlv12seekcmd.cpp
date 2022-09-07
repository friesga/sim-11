#include "rlv12seekcmd.h"
#include "rlv12/rlv12.h"

void RLV12SeekCmd::execute (RLV12 *controller, RL01_2 *unit)
{
    // Enter position mode
    // Heads locked on cyl
    unit->driveStatus_ = (unit->driveStatus_ & ~RLDS_M_STATE) | RLDS_LOCK; 
    return;
}

// No action to finish this command
void RLV12SeekCmd::finish (RLV12 *controller, RL01_2 *unit)
{}