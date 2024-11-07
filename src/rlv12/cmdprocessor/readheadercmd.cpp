#include "cmdprocessor.h"

//
// Perform a Read Header command for the specified unit with the 
// specified parameters.
//
u16 CmdProcessor::readHeaderCmd (RL01_02 *unit, RLV12Command &rlv12Command)
{
    u16 hdr[2];

    // Verify the unit is available and no Volume Check condition exists.
    if (!unit->available () || unit->volumeCheck ())
    {
        // EK-RLV12-TD-001 Figure 4-8 states a Operation Incomplete is
        // returned when the Read Header command fails.
        return RLV12const::CSR_OperationIncomplete;
    }

    // Put the header and CRC in the output buffer to be retrieved via the MPR
    // Simulate sequential rotation about the current track
    hdr[0] = controller_->dataBuffer_[0] = unit->readHeader (rlv12Command,
        RL01_02::HeadPositionProcedure::Rotate, controller_->dar_);
    hdr[1] = controller_->dataBuffer_[1] = 0;

    // Calculate header CRC
    controller_->dataBuffer_[2] = controller_->calcCRC (2, &hdr[0]);

    return 0;
}
