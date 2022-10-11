#include "cmdprocessor.h"

//
// Perform a Read Header command for the specified unit with the 
// specified parameters.
//
u16 CmdProcessor::readHeaderCmd (RL01_2 *unit, RLV12Command &rlv12Command)
{
    u16 hdr[2];

    // Verify the unit is available
    if (!unitAvailable (unit))
    {
        // Set spin error
        unit->driveStatus_ |= RLDS_SPE;

        // Flag error
        return RLCS_ERR | RLCS_INCMP;
    }

    // Guard against drive access while a seek is running
	std::lock_guard<std::mutex> guard{ unit->driveMutex_ };

    // Put the header and CRC in the output buffer to be retrieved via the MPR
    hdr[0] = controller_->rlxb_[0] = unit->currentDiskAddress_ & 0177777;
    hdr[1] = controller_->rlxb_[1] = 0;

    // Calculate header CRC
    controller_->rlxb_[2] = controller_->calcCRC (2, &hdr[0]);

    // Simulate sequential rotation about the current track
    updateHeadPosition (CmdProcessor::HeadPositionProcedure::Rotate, unit, 0);

    return 0;
}
