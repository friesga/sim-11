#include "cmdprocessor.h"

//
// Perform a Get Status command for the specified unit with the 
// specified parameters.
//
u16 CmdProcessor::getStatusCmd (RL01_2 *unit, RLV12Command &rlv12Command)
{
    // The Get Status bit (and ToDo: the Marker Bit) in the DAR must
    // be set.
    if (!(controller_->rlda & RLV12::DAR_GetStatus))
    {
        // Operation incomplete; set error 
        return RLV12::CSR_CompositeError | RLV12::CSR_OperationIncomplete;
    }

    // Reset errors
    // According to Table 4-6 in EK-RL012-UG-005 this also resets the
    // Volume Check condition
    if (controller_->rlda & RLV12::DAR_Reset)
        unit->driveStatus_ &= 
        ~(RLV12::MPR_GS_AnyError | RLV12::MPR_GS_VolumeCheck);

    // Develop drive state. The result is put in the output buffer, to be
    // retrieved via the MPR.
    controller_->rlxb_[0] = 
        (u16)(unit->driveStatus_ | 
        (unit->currentDiskAddress_ & RLV12::MPR_GS_HeadSelect));

    // Set Drive Type; a zero indicates an RL01; a one, an RL02.
    if (unit->rlStatus_ & RlStatus::UNIT_RL02)
        controller_->rlxb_[0] |= RLV12::MPR_GS_DriveType;

    // Check if unit is write-protected
    if (unit->rlStatus_ & RlStatus::UNIT_WLK || 
            unit->unitStatus_ & Status::UNIT_RO)
        controller_->rlxb_[0] |= RLV12::MPR_GS_WriteLock;

    if (unit->unitStatus_ & Status::UNIT_DIS || 
        unit->rlStatus_ & RlStatus::UNIT_OFFL)
    {
        controller_->rlxb_[0] |= RLV12::MPR_GS_DriveSelectError;
        unit->driveStatus_ |= RLV12::MPR_GS_DriveSelectError;
        return RLV12::CSR_OperationIncomplete;
    }
    return 0;
}