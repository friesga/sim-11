#include "rlv12.h"

//
// Perform a Get Status command for the specified unit with the 
// specified parameters.
//
u16 RLV12::getStatusCmd (RL01_02 *unit)
{
    // The Get Status bit (and ToDo: the Marker Bit) in the DAR must
    // be set.
    if (!(dar_ & RLV12const::DAR_GetStatus))
    {
        // Operation incomplete; set error 
        return RLV12const::CSR_CompositeError | RLV12const::CSR_OperationIncomplete;
    }

    // Reset errors
    // According to Table 4-6 in EK-RL012-UG-005 this also resets the
    // Volume Check condition
    if (dar_ & RLV12const::DAR_Reset)
        unit->driveStatus_ &= 
        ~(RLV12const::MPR_GS_AnyError | RLV12const::MPR_GS_VolumeCheck);

    // Develop drive state. The result is put in the output buffer, to be
    // retrieved via the MPR.
    dataBuffer_[0] = 
        (u16)(unit->driveStatus_ | 
        (unit->currentDiskAddress_ & RLV12const::MPR_GS_HeadSelect));

    // Set Drive Type; a zero indicates an RL01; a one, an RL02.
    if (unit->rlStatus_ & RlStatus::UNIT_RL02)
        dataBuffer_[0] |= RLV12const::MPR_GS_DriveType;

    // Check if unit is write-protected
    if (unit->rlStatus_ & RlStatus::UNIT_WLK || 
            unit->unitStatus_ & Status::UNIT_RO)
        dataBuffer_[0] |= RLV12const::MPR_GS_WriteLock;

    if (unit->unitStatus_ & Status::UNIT_DIS || 
        unit->rlStatus_ & RlStatus::UNIT_OFFL)
    {
        dataBuffer_[0] |= RLV12const::MPR_GS_DriveSelectError;
        unit->driveStatus_ |= RLV12const::MPR_GS_DriveSelectError;
        return RLV12const::CSR_OperationIncomplete;
    }
    return 0;
}