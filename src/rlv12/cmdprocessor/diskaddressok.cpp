#include "cmdprocessor.h"

bool CmdProcessor::diskAddressOk (RL01_02 *unit, RLV12Command &rlv12Command)
{
    if (RLV12const::getCylinder (unit->currentDiskAddress_) != 
            RLV12const::getCylinder (rlv12Command.diskAddress_) ||
            RLV12const::getSector (unit->currentDiskAddress_) >= RLV12const::sectorsPerSurface)
        // Bad cylinder or sector
        return false;

    return true;
}