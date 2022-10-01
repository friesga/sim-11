#include "cmdprocessor.h"

// A unit is available if it is not disconnected, is on-line and 
// a file is attached
bool CmdProcessor::unitAvailable (RL01_2 *unit)
{
    if (unit->unitStatus_ & Status::UNIT_DIS ||
            unit->rlStatus_ & RlStatus::UNIT_OFFL ||
            !(unit->unitStatus_ & Status::UNIT_ATT))
        return false;

    return true;
}