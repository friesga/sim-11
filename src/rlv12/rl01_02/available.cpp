#include "rl012.h"

// A unit is available if it is not disconnected, is on-line and 
// a file is attached
bool RL01_02::available ()
{
    if (unitStatus_ & Status::UNIT_DIS ||
            rlStatus_ & RlStatus::UNIT_OFFL ||
            !(unitStatus_ & Status::UNIT_ATT))
        return false;

    return true;
}