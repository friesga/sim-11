#include "cmdprocessor.h"

bool CmdProcessor::diskAddressOk (RL01_2 *unit, RLV12Command &rlv12Command)
{
    if (RLV12::getCylinder (unit->currentDiskAddress_) != 
            RLV12::getCylinder (rlv12Command.diskAddress_) ||
            RLV12::getSector (unit->currentDiskAddress_) >= RLV12::sectorsPerSurface)
        // Bad cylinder or sector
        return false;

    return true;
}