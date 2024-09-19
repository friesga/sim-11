#include "rl01_02.h"
#include "rlv12/rlv12.h"

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
void RL01_02::updateHeadPosition (HeadPositionProcedure procedure,
    s32 wordCount, u16 diskAddressRegister)
{
    switch (procedure)
    {
        case HeadPositionProcedure::Increment:
            currentDiskAddress_ = 
                (currentDiskAddress_ & ~RLV12const::DAR_Sector) |
                ((currentDiskAddress_ + 
                ((wordCount + (RLV12const::wordsPerSector - 1)) / 
                    RLV12const::wordsPerSector)) & RLV12const::DAR_Sector);
            break;

        case HeadPositionProcedure::Rotate:
            // Simulate sequential rotation about the current track
            // This functionality supports the Read Without Header Check
            // procedure, refer to EK-RLV12-UG-002, par. 5.8.
            currentDiskAddress_ =
                (currentDiskAddress_ & ~RLV12const::DAR_Sector) |
                ((currentDiskAddress_ + 1) & RLV12const::DAR_Sector);
            break;

        case HeadPositionProcedure::DiskAddressRegister:
            currentDiskAddress_ = diskAddressRegister;
            break;
    }

    if (RLV12const::getSector (currentDiskAddress_) >= 
            RLV12const::sectorsPerSurface)
        // Wrap to sector 0 
        currentDiskAddress_ &= ~RLV12const::DAR_Sector;
}