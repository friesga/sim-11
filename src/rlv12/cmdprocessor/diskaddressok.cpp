#include "cmdprocessor.h"

bool CmdProcessor::diskAddressOk (RL01_2 *unit, RLV12Command &rlv12Command)
{
    if (getCylinder (unit->currentDiskAddress_) != 
            getCylinder (rlv12Command.diskAddress_) ||
        getSector (unit->currentDiskAddress_) >= RL_NUMSC)
        // Bad cylinder or sector
        return false;

    return true;
}