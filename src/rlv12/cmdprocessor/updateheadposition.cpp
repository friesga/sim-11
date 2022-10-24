#include "cmdprocessor.h"

// Calculate and update the new head position from the current disk
// address and possibly the word count.
// 
// Three variants for the calculation are defined:
// - Set the sector to last sector read plus one,
// - Sequential rotation around the current track,
// - The value as specified in the DAR.
//
// In all cases the sector address wraps to zero if the calculated
// value exceeds the maximum sector address.
//
void CmdProcessor::updateHeadPosition 
    (CmdProcessor::HeadPositionProcedure procedure, 
     RL01_2 *unit, int32_t wordCount)
{
    switch (procedure)
    {
        case CmdProcessor::HeadPositionProcedure::Increment:
            unit->currentDiskAddress_ = 
                (unit->currentDiskAddress_ & ~RLV12::DAR_Sector) |
                ((unit->currentDiskAddress_ + 
                ((wordCount + (RLV12::wordsPerSector - 1)) / 
                    RLV12::wordsPerSector)) & RLV12::DAR_Sector);
            break;

        case CmdProcessor::HeadPositionProcedure::Rotate:
            // Simulate sequential rotation about the current track
            // This functionality supports the Read Without Header Check
            // procedure, refer to EK-RLV12-UG-002, par. 5.8.
            unit->currentDiskAddress_ =
                (unit->currentDiskAddress_ & ~RLV12::DAR_Sector) |
                ((unit->currentDiskAddress_ + 1) & RLV12::DAR_Sector);
            break;

        case CmdProcessor::HeadPositionProcedure::DiskAddressRegister:
            unit->currentDiskAddress_ = controller_->dar_;
            break;
    }

    if (RLV12::getSector (unit->currentDiskAddress_) >= 
            RLV12::sectorsPerSurface)
        // Wrap to sector 0 
        unit->currentDiskAddress_ &= ~RLV12::DAR_Sector;
}