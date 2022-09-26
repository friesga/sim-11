#include "rlv12readheadercmd.h"
#include "rlv12/rlv12.h"

#include <mutex>

using std::mutex;
using std::lock_guard;

void RLV12ReadHeaderCmd::execute (RLV12 *controller, RL01_2 *unit)
{
    u16 hdr[2];

    // Safeguard against simultaneous access
    lock_guard<mutex> guard{ controller->controllerMutex_ };

    hdr[0] = controller->rlmpr = unit->currentDiskAddress_ & 0177777;
    hdr[1] = controller->rlmpr1 = 0;

    // Calculate header CRC
    controller->rlmpr2 = controller->calcCRC (2, &hdr[0]);
    controller->setDone (0);

    // Simulate sequential rotation about the current track
    // This functionality supports the Read Without Header Check
    // procedure, refer to EK-RLV12-UG-002, par. 5.8.
    unit->currentDiskAddress_ =
        (unit->currentDiskAddress_ & ~RLDA_M_SECT) |
        ((unit->currentDiskAddress_ + 1) & RLDA_M_SECT);

    // End of track?
    if (getSector (unit->currentDiskAddress_) >= RL_NUMSC)
        // Wrap to 0
        unit->currentDiskAddress_ &= ~RLDA_M_SECT;                      
}

// No action to finish this command
void RLV12ReadHeaderCmd::finish (RLV12 *controller, RL01_2 *unit)
{}