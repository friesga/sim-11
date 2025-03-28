#include "cmdprocessor.h"

#include <chrono>
#include <future>

//
// Perform a Seek command for the specified unit with the 
// specified parameters.
//
u16 CmdProcessor::seekCmd (RL01_02 *unit, RLV12Command &rlv12Command)
{
    // Verify the unit is available and no Volume Check condition exists.
    if (!unit->available () || unit->volumeCheck ())
    {
        // EK-RLV12-TD-001 Figure 4-10 states a Operation Incomplete is
        // returned when the Seek command fails.
        // In this case the seeks in progress indicator has to be reset.
        unit->setSeekIncomplete ();
        return RLV12const::CSR_OperationIncomplete;
    }

    unit->seek (controller_->dar_);
    return 0;
}