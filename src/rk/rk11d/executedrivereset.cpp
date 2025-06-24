#include "rk11d.h"

// For a Drive Reset function, the controller directs the selected disk drive
// to move its head mechanism to cylinder address 000 and reset all active
// error status lines. To the controller, the Drive Reset function is the same
// as a Seek function, even to the manner in which the hardware poll logic is
// used; however, a Drive Reset function can take much longer than a Seek
// function to execute. The time required to complete a Drive Reset function
// depends on the physical position of the head mechanism at the time the
// function is initiated, and therefore may take up to a maximum of two
// seconds. (EK-RK11D-MM-002)
//
void RK11D::executeDriveReset (RKTypes::Function function)
{
    pollEventQueue_.push (StartPoll {});
    rk05Drives_[function.diskAddress.driveSelect]->seek (0);
}