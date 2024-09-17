#include "cmdprocessor.h"

#include <chrono>
#include <future>

//
// Perform a Seek command for the specified unit with the 
// specified parameters.
//
u16 CmdProcessor::seekCmd (RL01_02 *unit, RLV12Command &rlv12Command)
{
    // Verify the unit is available
    if (!unit->available ())
    {
        // Set seek time-out. Note that this status differs from
        // the status returned by data transfer commands if the
        // unit is unavailable.
        unit->driveStatus_ |= RLV12::MPR_GS_SeekTimeOut;

        // Flag error
        return RLV12::CSR_CompositeError | RLV12::CSR_OperationIncomplete;
    }

    unit->seek (controller_->dar_);
    return 0;
}